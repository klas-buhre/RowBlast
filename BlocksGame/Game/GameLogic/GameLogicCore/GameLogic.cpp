#include "GameLogic.hpp"

#include <limits>
#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "MathUtils.hpp"
#include "IAudio.hpp"

// Game includes.
#include "Field.hpp"
#include "Level.hpp"
#include "ScrollController.hpp"
#include "ExplosionParticleEffect.hpp"
#include "RowExplosionParticleEffect.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "PieceDropParticleEffect.hpp"
#include "BlastRadiusAnimation.hpp"
#include "GameHudController.hpp"
#include "CommonResources.hpp"

using namespace BlocksGame;

namespace {
    const auto maxRotateAdjustment {2};
    const Pht::IVec2 bombDetonationAreaSize {5, 5};
    const float halfColumn {0.5f};
    const auto landingNoMovementDurationFalling {1.0f};
    const auto landingMovementDurationFalling {4.0f};
    const auto cascadeWaitTime {0.25f};
    
    PieceBlocks CreatePieceBlocks(const FallingPiece& fallingPiece) {
        auto& pieceType {fallingPiece.GetPieceType()};
    
        return {
            pieceType.GetGrid(fallingPiece.GetRotation()),
            pieceType.GetGridNumRows(),
            pieceType.GetGridNumColumns()
        };
    }
    
    bool IsBomb(const Piece& pieceType) {
        return pieceType.IsBomb() || pieceType.IsRowBomb();
    }
}

GameLogic::GameLogic(Pht::IEngine& engine,
                     Field& field,
                     const ScrollController& scrollController,
                     const GameScene& gameScene,
                     ExplosionParticleEffect& explosionParticleEffect,
                     RowExplosionParticleEffect& rowExplosionParticleEffect,
                     FlyingBlocksAnimation& flyingBlocksAnimation,
                     PieceDropParticleEffect& pieceDropParticleEffect,
                     BlastRadiusAnimation& blastRadiusAnimation,
                     GameHudController& gameHudController,
                     const Settings& settings) :
    mEngine {engine},
    mField {field},
    mScrollController {scrollController},
    mExplosionParticleEffect {explosionParticleEffect},
    mRowExplosionParticleEffect {rowExplosionParticleEffect},
    mFlyingBlocksAnimation {flyingBlocksAnimation},
    mPieceDropParticleEffect {pieceDropParticleEffect},
    mBlastRadiusAnimation {blastRadiusAnimation},
    mGameHudController {gameHudController},
    mSettings {settings},
    mPreviousControlType {mSettings.mControlType},
    mFallingPieceAnimation {*this, mFallingPieceStorage},
    mGestureInputHandler {*this, mFallingPieceStorage},
    mClickInputHandler {engine, field, gameScene, *this},
    mFallingPiece {&mFallingPieceStorage} {}

void GameLogic::Init(const Level& level) {
    mLevel = &level;
    mGestureInputHandler.Init(level);
    mClickInputHandler.Init(level);
    
    if (mLevel->GetSpeed() > 0.0f) {
        mLandingNoMovementDuration = landingNoMovementDurationFalling;
        mLandingMovementDuration = landingMovementDurationFalling;
    } else {
        mLandingNoMovementDuration = std::numeric_limits<float>::max();
        mLandingMovementDuration = std::numeric_limits<float>::max();
    }
    
    mCascadeState = CascadeState::NotCascading;

    RemoveFallingPiece();
    mFallingPieceInitReason = FallingPieceInitReason::NextMove;
    mFallingPieceInitType = nullptr;
    
    mMovesLeft = mLevel->GetNumMoves();
    mMovesUsed = 0;
    
    mCurrentMove = MoveData {};
    auto& nextPieceGenerator {mCurrentMove.mNextPieceGenerator};
    nextPieceGenerator.Init(mLevel->GetPieceTypes());
    mCurrentMove.mSelectablePieces[1] = &nextPieceGenerator.GetNext();
    mCurrentMove.mSelectablePieces[0] = &nextPieceGenerator.GetNext();
    mCurrentMoveInitialState = mCurrentMove;
    mPreviousMoveInitialState = mCurrentMoveInitialState;
    
    mFallingPieceStorage.ResetBetweenGames();
    
    UpdateLevelProgress();
}

GameLogic::Result GameLogic::Update(bool shouldUpdateLogic) {
    if (shouldUpdateLogic) {
        HandleCascading();
        
        if (mCascadeState != CascadeState::NotCascading) {
            return Result::None;
        }
        
        HandleControlTypeChange();
        
        if (mFallingPieceInitReason != FallingPieceInitReason::None) {
            auto result {InitFallingPiece()};
            mFallingPieceInitReason = FallingPieceInitReason::None;
            
            if (result != Result::None) {
                return result;
            }
        }
        
        mFallingPiece->UpdateTime(mEngine.GetLastFrameSeconds());
        UpdateFallingPieceYpos();
    }
    
    return HandleInput();
}

GameLogic::Result GameLogic::InitFallingPiece() {
    assert(mFallingPieceInitReason != FallingPieceInitReason::None);
    
    if (mBlastRadiusAnimation.IsActive()) {
        mBlastRadiusAnimation.Stop();
    }
    
    UpdateLevelProgress();
    
    if (IsLevelCompleted()) {
        return Result::LevelCompleted;
    }
    
    mFallingPiece = &mFallingPieceStorage;
    CalculatePieceType();
    auto initPosition {CalculateFallingPieceInitPos()};
    mFallingPiece->Init(*mCurrentMove.mPieceType, initPosition, mLevel->GetSpeed());
    
    ManageMoveHistory();
    
    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(*mFallingPiece),
                                                mFallingPiece->GetIntPosition());
    
    if (mGhostPieceRow > mFallingPiece->GetPosition().y) {
        RemoveFallingPiece();
        return Result::GameOver;
    }
    
    if (mCurrentMove.mPieceType->IsBomb() && mSettings.mControlType == ControlType::Gesture) {
        StartBlastRadiusAnimationAtGhostPiece();
    }
    
    if (mSettings.mControlType == ControlType::Click) {
        std::cout << "Calculating moves" << std::endl;
        
        mClickInputHandler.CalculateMoves(*mFallingPiece);
        mClickInputHandler.CreateNewSetOfVisibleMoves();
    }
    
    if (mMovesLeft == 0) {
        return Result::OutOfMoves;
    }
    
    return Result::None;
}

void GameLogic::CalculatePieceType() {
    if (mFallingPieceInitType) {
        mCurrentMove.mPieceType = mFallingPieceInitType;
        mFallingPieceInitType = nullptr;
    } else {
        mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[1];
        mCurrentMove.mSelectablePieces[1] = mCurrentMove.mSelectablePieces[0];
        mCurrentMove.mSelectablePieces[0] = &mCurrentMove.mNextPieceGenerator.GetNext();
    }
}

bool GameLogic::IsLevelCompleted() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            if (mNumLevelBlocksLeft == 0) {
                return true;
            }
            break;
        case Level::Objective::Build:
            if (mField.AccordingToBlueprintHeight() == mField.GetNumRows()) {
                return true;
            }
            break;
    }
    
    return false;
}

void GameLogic::ManageMoveHistory() {
    mCurrentMove.mId = mFallingPiece->GetId();
    
    switch (mFallingPieceInitReason) {
        case FallingPieceInitReason::NextMove:
            ++mMovesUsed;
            if (GetMovesUsedIncludingCurrent() > 1) {
                mPreviousMoveInitialState = mCurrentMoveInitialState;
            }
            mCurrentMoveInitialState = mCurrentMove;
            if (GetMovesUsedIncludingCurrent() == 1) {
                mPreviousMoveInitialState = mCurrentMoveInitialState;
            }
            break;
        case FallingPieceInitReason::UndoMove:
            mCurrentMoveInitialState = mCurrentMove;
            mPreviousMoveInitialState = mCurrentMoveInitialState;
            break;
        case FallingPieceInitReason::None:
        case FallingPieceInitReason::Switch:
            break;
    }
}

void GameLogic::RemoveFallingPiece() {
    mFallingPiece = nullptr;
}

Pht::Vec2 GameLogic::CalculateFallingPieceInitPos() {
    auto& pieceType {*mCurrentMove.mPieceType};
    auto startXPos {mField.GetNumColumns() / 2 - pieceType.GetGridNumColumns() / 2};
    
    if (mFallingPieceInitReason == FallingPieceInitReason::Switch && mLevel->GetSpeed() > 0.0f) {
        return Pht::Vec2 {startXPos + halfColumn, mFallingPiece->GetPosition().y};
    }
    
    auto topRowInScreen {
        static_cast<int>(mScrollController.GetLowestVisibleRow()) + mField.GetNumRowsInOneScreen() - 1
    };
    
    switch (mScrollController.GetState()) {
        case ScrollController::State::ScrollingSlowly:
        case ScrollController::State::LastScrollStep:
            if (mScrollController.GetScrollDirection() == ScrollController::ScrollDirection::Up) {
                ++topRowInScreen;
            }
            break;
        default:
            break;
    }
    
    auto desiredUpperPos {topRowInScreen - 3 + pieceType.GetNumEmptyTopRows()};
    
    if (mLevel->GetSpeed() > 0.0f) {
        ++desiredUpperPos;
    }

    auto startYPos {desiredUpperPos - pieceType.GetGridNumRows() + 1};
    return Pht::Vec2 {startXPos + halfColumn, static_cast<float>(startYPos)};
}

void GameLogic::HandleCascading() {
    switch (mCascadeState) {
        case CascadeState::Cascading:
            mField.ManageWelds();
            if (mField.AnyFilledRows()) {
                mCascadeWaitTime = 0.0f;
                mCascadeState = CascadeState::WaitingToClearLine;
            } else {
                mCascadeState = CascadeState::NotCascading;
            }
            break;
        case CascadeState::WaitingToClearLine:
            mCascadeWaitTime += mEngine.GetLastFrameSeconds();
            if (mCascadeWaitTime > cascadeWaitTime && !mScrollController.IsScrolling()) {
                auto removedSubCells {mField.RemoveFilledRows()};
                mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                UpdateLevelProgress();
                PullDownLoosePieces();
            }
            break;
        case CascadeState::NotCascading:
            break;
    }
}

void GameLogic::HandleControlTypeChange() {
    if (mFallingPiece == nullptr) {
        return;
    }
    
    if (mSettings.mControlType != mPreviousControlType) {
        switch (mSettings.mControlType) {
            case ControlType::Click:
                if (mCurrentMove.mPieceType->IsBomb()) {
                    mBlastRadiusAnimation.Stop();
                }
                mClickInputHandler.CalculateMoves(*mFallingPiece);
                mClickInputHandler.CreateNewSetOfVisibleMoves();
                break;
            case ControlType::Gesture:
                if (mCurrentMove.mPieceType->IsBomb()) {
                    StartBlastRadiusAnimationAtGhostPiece();
                }
                break;
        }
    }
    
    mPreviousControlType = mSettings.mControlType;
}

void GameLogic::UpdateLevelProgress() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mNumLevelBlocksLeft = mField.CalculateNumLevelBlocks();
            break;
        case Level::Objective::Build:
            mNumEmptyBlueprintSlotsLeft = mField.CalculateNumEmptyBlueprintSlots();
            break;
    }
}

void GameLogic::NextMove() {
    RemoveFallingPiece();
    mFallingPieceInitReason = FallingPieceInitReason::NextMove;
    --mMovesLeft;
    UpdateLevelProgress();
}

bool GameLogic::IsUndoMovePossible() const {
    return mCurrentMove.mId != mPreviousMoveInitialState.mId;
}

void GameLogic::UndoMove() {
    if (!IsUndoMovePossible()) {
        return;
    }
    
    mField.RestorePreviousState();
    mCurrentMove = mPreviousMoveInitialState;
    mFallingPieceInitType = mPreviousMoveInitialState.mPieceType;
    mFallingPieceInitReason = FallingPieceInitReason::UndoMove;
    RemoveFallingPiece();
    ++mMovesLeft;
    --mMovesUsed;
    UpdateLevelProgress();
}

int GameLogic::GetMovesUsedIncludingCurrent() const {
    return mMovesUsed;
}

void GameLogic::UpdateFallingPieceYpos() {
    auto newYPosition {mFallingPiece->GetPosition().y -
                       mFallingPiece->GetSpeed() * mEngine.GetLastFrameSeconds()};

    switch (mFallingPiece->GetState()) {
        case FallingPiece::State::Falling:
        case FallingPiece::State::BeingDraggedDown:
            if (newYPosition < mGhostPieceRow) {
                mFallingPiece->GoToLandingState(mGhostPieceRow);
            } else {
                mFallingPiece->SetY(newYPosition);
                if (mSettings.mControlType == ControlType::Click) {
                    if (mFallingPiece->GetPreviousIntY() != static_cast<int>(newYPosition)) {
                        mClickInputHandler.UpdateMoves(*mFallingPiece);
                    }
                }
            }
            break;
        case FallingPiece::State::Landing:
            if (mFallingPiece->GetPosition().y != mGhostPieceRow) {
                mFallingPiece->GoToFallingState(mLevel->GetSpeed());
            } else {
                auto lastFrameSeconds {mEngine.GetLastFrameSeconds()};
                mFallingPiece->IncreaseTimeSinceTouchdown(lastFrameSeconds);
                mFallingPiece->IncreaseTimeSinceMovement(lastFrameSeconds);
                if (mFallingPiece->GetTimeSinceTouchdown() > mLandingMovementDuration ||
                    mFallingPiece->GetTimeSinceMovement() > mLandingNoMovementDuration) {
                    LandFallingPiece(false);
                }
            }
            break;
    }
}

void GameLogic::DropFallingPiece() {
    bool startParticleEffect {mFallingPiece->GetPosition().y > mGhostPieceRow};
    mFallingPiece->SetY(mGhostPieceRow);
    LandFallingPiece(startParticleEffect);
}

void GameLogic::StartFallingPieceAnimation(const Movement& lastMovement) {
    mFallingPieceAnimation.Start(lastMovement);
}

void GameLogic::OnFallingPieceAnimationFinished(bool startParticleEffect) {
    LandFallingPiece(startParticleEffect);
}

void GameLogic::LandFallingPiece(bool startParticleEffect) {
    if (startParticleEffect) {
        mPieceDropParticleEffect.StartEffect(*mFallingPiece);
    }
    
    if (IsBomb(mFallingPiece->GetPieceType())) {
        DetonateBomb();
    } else {
        mField.LandFallingPiece(*mFallingPiece);
        
        if (mLevel->GetObjective() == Level::Objective::Clear) {
            auto removedSubCells {mField.RemoveFilledRows()};

            if (removedSubCells.Size() > 0) {
                mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                PullDownLoosePieces();
            }
        }
    }
    
    NextMove();
}

void GameLogic::DetonateBomb() {
    mEngine.GetAudio().PlaySound(CommonResources::mBombSound);
    
    auto intDetonationPos {mFallingPiece->GetIntPosition() + Pht::IVec2{1, 1}};
    auto detonationPos {mFallingPiece->GetRenderablePosition() + Pht::Vec2{1.0f, 1.0f}};
    
    if (mFallingPiece->GetPieceType().IsRowBomb()) {
        mRowExplosionParticleEffect.StartExplosion(detonationPos);
        
        auto removedSubCells {mField.RemoveRow(intDetonationPos.y)};

        if (removedSubCells.Size() > 0) {
            mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
        }
    } else {
        mExplosionParticleEffect.StartExplosion(detonationPos);
        
        Pht::IVec2 detonationAreaPos {intDetonationPos - Pht::IVec2{2, 2}};
        auto removedSubCells {mField.RemoveAreaOfSubCells(detonationAreaPos, bombDetonationAreaSize)};

        if (removedSubCells.Size() > 0) {
            mFlyingBlocksAnimation.AddBlocks(removedSubCells, intDetonationPos);
        }
        
        if (mBlastRadiusAnimation.IsActive()) {
            mBlastRadiusAnimation.Stop();
        }
    }
    
    PullDownLoosePieces();
}

void GameLogic::PullDownLoosePieces() {
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    mField.PullDownLoosePieces();
    
    mCascadeState = CascadeState::Cascading;
}

void GameLogic::RotateFallingPiece() {
    auto& pieceType {mFallingPiece->GetPieceType()};

    auto newRotation {
        static_cast<Rotation>((static_cast<int>(mFallingPiece->GetRotation()) + 1) % 4)
    };
    
    PieceBlocks pieceBlocks {
        pieceType.GetGrid(newRotation),
        pieceType.GetGridNumRows(),
        pieceType.GetGridNumColumns()
    };
    
    auto position {mFallingPiece->GetIntPosition()};
    
    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, position, Pht::IVec2{0, 0}, false);

    if (collisionResult.mIsCollision == IsCollision::Yes) {
        auto collisionDirection {
            CollisionDetection::CalculateCollisionDirection(collisionResult.mCollisionPoints,
                                                            pieceBlocks,
                                                            newRotation,
                                                            position,
                                                            mField)
        };
        
        RotatateAndAdjustPosition(newRotation, pieceBlocks, position, collisionDirection);
    } else {
        mFallingPiece->SetRotation(newRotation);
    }

    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(*mFallingPiece),
                                                mFallingPiece->GetIntPosition());
}

void GameLogic::RotatateAndAdjustPosition(Rotation newRotation,
                                          const PieceBlocks& pieceBlocks,
                                          const Pht::IVec2& position,
                                          Direction collisionDirection) {
    switch (collisionDirection) {
        case Direction::Right: {
            auto freeXPos {mField.DetectFreeSpaceLeft(pieceBlocks, position)};
            if (position.x - freeXPos <= maxRotateAdjustment) {
                mFallingPiece->SetX(freeXPos + halfColumn);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Left: {
            auto freeXPos {mField.DetectFreeSpaceRight(pieceBlocks, position)};
            if (freeXPos - position.x <= maxRotateAdjustment) {
                mFallingPiece->SetX(freeXPos + halfColumn);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Up: {
            auto freeYPos {mField.DetectFreeSpaceDown(pieceBlocks, position)};
            if (position.y - freeYPos <= maxRotateAdjustment) {
                mFallingPiece->SetY(freeYPos);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Down: {
            auto freeYPos {mField.DetectFreeSpaceUp(pieceBlocks, position)};
            if (freeYPos - position.y <= maxRotateAdjustment) {
                mFallingPiece->SetY(freeYPos);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        default:
            break;
    }
}

void GameLogic::SwitchPiece() {
    mFallingPieceInitType = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mSelectablePieces[1] = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mSelectablePieces[0] = &mFallingPiece->GetPieceType();
    mFallingPieceInitReason = FallingPieceInitReason::Switch;
}

void GameLogic::RotatePieceOrDetonateBomb() {
    std::cout << "tap" << std::endl;
    
    auto& pieceType {mFallingPiece->GetPieceType()};
    
    if (IsBomb(pieceType)) {
        DetonateBomb();
        NextMove();
    } else if (pieceType.CanRotateAroundZ()) {
        RotateFallingPiece();
    }
}

void GameLogic::SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) {
    auto pieceBlocks {CreatePieceBlocks(*mFallingPiece)};

    if (fallingPieceNewX - mFallingPiece->GetPosition().x > 0.0f) {
        auto collisionRight {
            mField.DetectCollisionRight(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn
        };
        
        mFallingPiece->SetX(std::fmin(collisionRight, fallingPieceNewX));
    } else {
        auto collisionLeft {
            mField.DetectCollisionLeft(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn
        };
        
        mFallingPiece->SetX(std::fmax(collisionLeft, fallingPieceNewX));
    }

    mGhostPieceRow = mField.DetectCollisionDown(pieceBlocks, mFallingPiece->GetIntPosition());
    
    if (mBlastRadiusAnimation.IsActive()) {
        SetBlastRadiusAnimationPositionAtGhostPiece();
    }
}

int GameLogic::GetGhostPieceRow() const {
    return mGhostPieceRow;
}

void GameLogic::StartBlastRadiusAnimationAtGhostPiece() {
    mBlastRadiusAnimation.Start();
    SetBlastRadiusAnimationPositionAtGhostPiece();
}

void GameLogic::SetBlastRadiusAnimationPositionAtGhostPiece() {
    Pht::Vec2 blastRadiusAnimationPos {
        mFallingPiece->GetRenderablePosition().x + 1.0f,
        static_cast<float>(mGhostPieceRow) + 1.0f
    };

    mBlastRadiusAnimation.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StartBlastRadiusAnimation(const Pht::IVec2& position) {
    mBlastRadiusAnimation.Start();
    
    Pht::Vec2 blastRadiusAnimationPos {
        static_cast<float>(position.x) + 1.0f,
        static_cast<float>(position.y) + 1.0f
    };

    mBlastRadiusAnimation.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StopBlastRadiusAnimation() {
    mBlastRadiusAnimation.Stop();
}

GameLogic::Result GameLogic::HandleInput() {
    auto result {Result::None};
    auto& input {mEngine.GetInput()};
    
    while (input.HasEvents()) {
        auto& event {input.GetNextEvent()};
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent {event.GetTouchEvent()};
                switch (mGameHudController.OnTouch(touchEvent)) {
                    case GameHudController::Result::None:
                        ForwardTouchToInputHandler(touchEvent);
                        break;
                    case GameHudController::Result::ClickedSwitch:
                        if (mFallingPiece) {
                            SwitchPiece();
                        }
                        break;
                    case GameHudController::Result::ClickedPause:
                        result = Result::Pause;
                        break;
                    case GameHudController::Result::TouchStartedOverButton:
                        break;
                }
                break;
            }
            default:
                assert(false);
                break;
        }
        
        input.PopNextEvent();
        
        if (result == Result::Pause) {
            break;
        }
    }
    
    return result;
}

void GameLogic::ForwardTouchToInputHandler(const Pht::TouchEvent& touchEvent) {
    if (mFallingPiece == nullptr) {
        return;
    }
    
    switch (mSettings.mControlType) {
        case ControlType::Click:
            mClickInputHandler.HandleTouch(touchEvent);
            break;
        case ControlType::Gesture:
            mGestureInputHandler.HandleTouch(touchEvent);
            break;
    }
}