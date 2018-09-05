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
#include "Level.hpp"
#include "ScrollController.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "FlashingBlocksAnimation.hpp"
#include "CollapsingFieldAnimation.hpp"
#include "EffectManager.hpp"
#include "PieceDropParticleEffect.hpp"
#include "BlastRadiusAnimation.hpp"
#include "FallingPieceScaleAnimation.hpp"
#include "ComboTextAnimation.hpp"
#include "GameHudController.hpp"
#include "Tutorial.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto maxRotateAdjustment {2};
    const Pht::IVec2 bombDetonationAreaSize {5, 5};
    constexpr float halfColumn {0.5f};
    constexpr auto landingNoMovementDurationFalling {1.0f};
    constexpr auto landingMovementDurationFalling {4.0f};
    constexpr auto cascadeWaitTime {0.23f};
    constexpr auto cameraShakeNumRowsLimit {5};

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
    
    int CalcNumRemovedRows(const Field::RemovedSubCells& removedSubCells) {
        Pht::Optional<int> previousRowIndex;
        auto numRemovedRows {0};
        
        for (auto& subCell: removedSubCells) {
            auto rowIndex {subCell.mGridPosition.y};
            
            if (previousRowIndex.HasValue()) {
                if (previousRowIndex.GetValue() != rowIndex) {
                    previousRowIndex = rowIndex;
                    ++numRemovedRows;
                }
            } else {
                previousRowIndex = rowIndex;
                numRemovedRows = 1;
            }
        }
        
        return numRemovedRows;
    }
}

GameLogic::GameLogic(Pht::IEngine& engine,
                     Field& field,
                     ScrollController& scrollController,
                     const GameScene& gameScene,
                     EffectManager& effectManager,
                     FlyingBlocksAnimation& flyingBlocksAnimation,
                     FlashingBlocksAnimation& flashingBlocksAnimation,
                     CollapsingFieldAnimation& collapsingFieldAnimation,
                     PieceDropParticleEffect& pieceDropParticleEffect,
                     BlastRadiusAnimation& blastRadiusAnimation,
                     FallingPieceScaleAnimation& fallingPieceScaleAnimation,
                     ComboTextAnimation& comboTextAnimation,
                     GameHudController& gameHudController,
                     Tutorial& tutorial,
                     const Settings& settings) :
    mEngine {engine},
    mField {field},
    mScrollController {scrollController},
    mFlyingBlocksAnimation {flyingBlocksAnimation},
    mFlashingBlocksAnimation {flashingBlocksAnimation},
    mCollapsingFieldAnimation {collapsingFieldAnimation},
    mEffectManager {effectManager},
    mPieceDropParticleEffect {pieceDropParticleEffect},
    mBlastRadiusAnimation {blastRadiusAnimation},
    mFallingPieceScaleAnimation {fallingPieceScaleAnimation},
    mComboTextAnimation {comboTextAnimation},
    mGameHudController {gameHudController},
    mTutorial {tutorial},
    mSettings {settings},
    mControlType {mSettings.mControlType},
    mFieldExplosionsStates {engine, field, effectManager, flyingBlocksAnimation},
    mFallingPieceAnimation {*this, mFallingPieceStorage},
    mGestureInputHandler {*this, mFallingPieceStorage},
    mClickInputHandler {engine, field, gameScene, *this, tutorial},
    mFallingPiece {&mFallingPieceStorage} {
    
    scrollController.SetGameLogic(*this);
}

void GameLogic::Init(const Level& level) {
    mLevel = &level;
    mControlType = mTutorial.IsGestureControlsAllowed() ? mSettings.mControlType : ControlType::Click;
    mGestureInputHandler.Init(level);
    mClickInputHandler.Init(level);
    
    if (mLevel->GetSpeed() > 0.0f) {
        mLandingNoMovementDuration = landingNoMovementDurationFalling;
        mLandingMovementDuration = landingMovementDurationFalling;
    } else {
        mLandingNoMovementDuration = std::numeric_limits<float>::max();
        mLandingMovementDuration = std::numeric_limits<float>::max();
    }
    
    mState = State::LogicUpdate;
    mCascadeState = CascadeState::NotCascading;

    RemoveFallingPiece();
    mFallingPieceSpawnReason = FallingPieceSpawnReason::NextMove;
    mFallingPieceSpawnType = nullptr;
    
    mMovesLeft = mLevel->GetNumMoves();
    mMovesUsed = 0;
    
    mCurrentMove = MoveData {};
    auto& nextPieceGenerator {mCurrentMove.mNextPieceGenerator};
    nextPieceGenerator.Init(mLevel->GetPieceTypes(), mLevel->GetPieceSequence());
    mCurrentMove.mSelectablePieces[1] = &nextPieceGenerator.GetNext();
    mCurrentMove.mSelectablePieces[0] = &nextPieceGenerator.GetNext();
    mCurrentMoveInitialState = mCurrentMove;
    mPreviousMoveInitialState = mCurrentMoveInitialState;
    
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
    
    mFallingPieceStorage.ResetBetweenGames();
    
    UpdateLevelProgress();
}

GameLogic::Result GameLogic::Update(bool shouldUpdateLogic) {
    switch (mState) {
        case State::LogicUpdate:
            if (shouldUpdateLogic) {
                HandleCascading();
                if (mCascadeState != CascadeState::NotCascading) {
                    return Result::None;
                }
                HandleControlTypeChange();
                if (mFallingPieceSpawnReason != FallingPieceSpawnReason::None) {
                    auto result {SpawnFallingPiece()};
                    mFallingPieceSpawnReason = FallingPieceSpawnReason::None;
                    if (result != Result::None) {
                        return result;
                    }
                }
                mFallingPiece->UpdateTime(mEngine.GetLastFrameSeconds());
                UpdateFallingPieceYpos();
            }
            break;
        case State::FieldExplosions:
            UpdateFieldExplosionsStates();
            break;
    }
    
    return HandleInput();
}

GameLogic::Result GameLogic::SpawnFallingPiece() {
    assert(mFallingPieceSpawnReason != FallingPieceSpawnReason::None);
    
    if (mBlastRadiusAnimation.IsActive()) {
        mBlastRadiusAnimation.Stop();
    }
    
    UpdateLevelProgress();
    
    if (IsLevelCompleted()) {
        return Result::LevelCompleted;
    }
    
    mFallingPiece = &mFallingPieceStorage;
    SetPieceType();
    auto spawnPosition {
        CalculateFallingPieceSpawnPos(*mCurrentMove.mPieceType, mFallingPieceSpawnReason)
    };
    mFallingPiece->Spawn(*mCurrentMove.mPieceType, spawnPosition, mLevel->GetSpeed());
    
    ManageMoveHistory();
    
    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(*mFallingPiece),
                                                mFallingPiece->GetIntPosition());
    
    if (mGhostPieceRow > mFallingPiece->GetPosition().y) {
        RemoveFallingPiece();
        return Result::GameOver;
    }
    
    if (mCurrentMove.mPieceType->IsBomb() && mControlType == ControlType::Gesture) {
        StartBlastRadiusAnimationAtGhostPiece();
    }
    
    if (mControlType == ControlType::Click) {
        std::cout << "Calculating moves" << std::endl;
        
        mClickInputHandler.CalculateMoves(*mFallingPiece, GetMovesUsedIncludingCurrent() - 1);
        mClickInputHandler.CreateNewSetOfVisibleMoves();
    }
    
    mFallingPieceScaleAnimation.Start();
    
    if (mMovesLeft == 0) {
        return Result::OutOfMoves;
    }
    
    return Result::None;
}

void GameLogic::SetPieceType() {
    if (mFallingPieceSpawnType) {
        mCurrentMove.mPieceType = mFallingPieceSpawnType;
        mFallingPieceSpawnType = nullptr;
    } else {
        mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[1];
        mCurrentMove.mSelectablePieces[1] = mCurrentMove.mSelectablePieces[0];
        mCurrentMove.mSelectablePieces[0] = &mCurrentMove.mNextPieceGenerator.GetNext();
        mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::NextPieceAndSwitch;
    }
}

const Piece* GameLogic::GetPieceType() const {
    if (mFallingPiece || mFallingPieceSpawnReason == FallingPieceSpawnReason::UndoMove) {
        return mCurrentMove.mPieceType;
    }
    
    return nullptr;
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
    
    switch (mFallingPieceSpawnReason) {
        case FallingPieceSpawnReason::NextMove:
            ++mMovesUsed;
            if (GetMovesUsedIncludingCurrent() > 1) {
                mPreviousMoveInitialState = mCurrentMoveInitialState;
            }
            mCurrentMoveInitialState = mCurrentMove;
            if (GetMovesUsedIncludingCurrent() == 1) {
                mPreviousMoveInitialState = mCurrentMoveInitialState;
            }
            mTutorial.OnNewMove(GetMovesUsedIncludingCurrent());
            break;
        case FallingPieceSpawnReason::UndoMove:
            mCurrentMoveInitialState = mCurrentMove;
            mPreviousMoveInitialState = mCurrentMoveInitialState;
            mTutorial.OnNewMove(GetMovesUsedIncludingCurrent());
            break;
        case FallingPieceSpawnReason::Switch:
            mTutorial.OnSwitchPiece(GetMovesUsedIncludingCurrent(), mFallingPiece->GetPieceType());
            break;
        case FallingPieceSpawnReason::None:
            break;
    }
}

void GameLogic::RemoveFallingPiece() {
    mFallingPiece = nullptr;
}

Pht::Vec2 GameLogic::CalculateFallingPieceSpawnPos(const Piece& pieceType,
                                                   FallingPieceSpawnReason fallingPieceSpawnReason) {
    auto startXPos {mField.GetNumColumns() / 2 - pieceType.GetGridNumColumns() / 2};
    
    if (fallingPieceSpawnReason == FallingPieceSpawnReason::Switch && mLevel->GetSpeed() > 0.0f) {
        auto pieceNumEmptyBottompRows {pieceType.GetDimensions(Rotation::Deg0).mYmin};
        
        auto previousPieceNumEmptyBottomRows {
            mFallingPiece->GetPieceType().GetDimensions(Rotation::Deg0).mYmin
        };
        
        auto yAdjust {
            static_cast<float>(previousPieceNumEmptyBottomRows - pieceNumEmptyBottompRows)
        };
        
        return Pht::Vec2 {startXPos + halfColumn, mFallingPiece->GetPosition().y + yAdjust};
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
    
    auto& pieceDimensions {pieceType.GetDimensions(Rotation::Deg0)};
    auto pieceNumEmptyTopRows {pieceType.GetGridNumRows() - pieceDimensions.mYmax - 1};
    auto desiredUpperPos {topRowInScreen - 3 + pieceNumEmptyTopRows};
    
    if (mLevel->GetSpeed() > 0.0f) {
        ++desiredUpperPos;
    }

    auto startYPos {desiredUpperPos - pieceType.GetGridNumRows() + 1};
    return Pht::Vec2 {startXPos + halfColumn, static_cast<float>(startYPos)};
}

void GameLogic::HandleCascading() {
    switch (mCascadeState) {
        case CascadeState::Cascading:
            if (mField.AnyFilledRows()) {
                mCascadeWaitTime = 0.0f;
                mCascadeState = CascadeState::WaitingToClearLine;
            } else {
                mCascadeState = CascadeState::NotCascading;
                mField.ManageWelds();
            }
            break;
        case CascadeState::WaitingToClearLine:
            mCascadeWaitTime += mEngine.GetLastFrameSeconds();
            if ((mCascadeWaitTime > cascadeWaitTime || mCollapsingFieldAnimation.IsInactive())
                && !mScrollController.IsScrolling()) {

                auto removedSubCells {mField.ClearFilledRows()};
                
                if (CalcNumRemovedRows(removedSubCells) >= cameraShakeNumRowsLimit) {
                    mEffectManager.StartSmallCameraShake();
                }

                mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                UpdateLevelProgress();
                mCollapsingFieldAnimation.GoToInactiveState();
                mCollapsingFieldAnimation.ResetBlockAnimations();
                RemoveClearedRowsAndPullDownLoosePieces();
            }
            break;
        case CascadeState::NotCascading:
            break;
    }
}

void GameLogic::UpdateFieldExplosionsStates() {
    if (mFieldExplosionsStates.Update() == FieldExplosionsStates::State::Inactive) {
        mState = State::LogicUpdate;
        UpdateLevelProgress();
        RemoveClearedRowsAndPullDownLoosePieces();
    }
}

void GameLogic::HandleControlTypeChange() {
    if (mFallingPiece == nullptr) {
        return;
    }
    
    if (mSettings.mControlType != mControlType && mTutorial.IsGestureControlsAllowed()) {
        switch (mSettings.mControlType) {
            case ControlType::Click:
                if (mCurrentMove.mPieceType->IsBomb()) {
                    mBlastRadiusAnimation.Stop();
                }
                mClickInputHandler.CalculateMoves(*mFallingPiece,
                                                  GetMovesUsedIncludingCurrent() - 1);
                mClickInputHandler.CreateNewSetOfVisibleMoves();
                break;
            case ControlType::Gesture:
                if (mCurrentMove.mPieceType->IsBomb()) {
                    StartBlastRadiusAnimationAtGhostPiece();
                }
                break;
        }
        
        mControlType = mSettings.mControlType;
    }
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
    mFallingPieceSpawnReason = FallingPieceSpawnReason::NextMove;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::RemoveActivePiece;
    --mMovesLeft;
    UpdateLevelProgress();
}

bool GameLogic::IsUndoMovePossible() const {
    return mCurrentMove.mId != mPreviousMoveInitialState.mId && mMovesUsed > 1 &&
           mTutorial.IsUndoMoveAllowed(GetMovesUsedIncludingCurrent());
}

void GameLogic::UndoMove() {
    if (!IsUndoMovePossible()) {
        return;
    }
    
    mField.RestorePreviousState();
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());

    mCurrentMove = mPreviousMoveInitialState;
    mFallingPieceSpawnType = mPreviousMoveInitialState.mPieceType;
    mFallingPieceSpawnReason = FallingPieceSpawnReason::UndoMove;
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
                if (mControlType == ControlType::Click) {
                    if (mFallingPiece->GetPreviousIntY() != static_cast<int>(newYPosition)) {
                        mClickInputHandler.UpdateMoves(*mFallingPiece,
                                                       GetMovesUsedIncludingCurrent() - 1);
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
    bool finalMovementWasADrop {mFallingPiece->GetPosition().y > mGhostPieceRow};
    mFallingPiece->SetY(mGhostPieceRow);
    LandFallingPiece(finalMovementWasADrop);
}

void GameLogic::SelectMove(const Move& move) {
    mTutorial.OnSelectMove();
    mFallingPieceAnimation.Start(*move.mLastMovement);
}

void GameLogic::OnFallingPieceAnimationFinished(bool finalMovementWasADrop) {
    LandFallingPiece(finalMovementWasADrop);
}

void GameLogic::LandFallingPiece(bool finalMovementWasADrop) {
    mField.SaveState();
    
    if (finalMovementWasADrop) {
        mPieceDropParticleEffect.StartEffect(*mFallingPiece);
    }
    
    auto& pieceType {mFallingPiece->GetPieceType()};
    
    if (IsBomb(pieceType)) {
        DetonateDroppedBomb();
    } else {
        auto impactedLevelBombs {
            mField.DetectImpactedBombs(CreatePieceBlocks(*mFallingPiece),
                                       mFallingPiece->GetIntPosition())
        };
        
        auto startBounceAnimation {false};
        
        if (finalMovementWasADrop) {
            startBounceAnimation = true;
            mCollapsingFieldAnimation.GoToBlocksBouncingState();
        }
        
        mField.LandFallingPiece(*mFallingPiece, startBounceAnimation);
        mFlashingBlocksAnimation.Start(pieceType.GetColor());
        DetonateImpactedLevelBombs(impactedLevelBombs);
        
        if (mLevel->GetObjective() == Level::Objective::Clear) {
            auto removedSubCells {mField.ClearFilledRows()};

            if (!removedSubCells.IsEmpty()) {
                mComboTextAnimation.Start(ComboTextAnimation::Message::Combo);

                if (CalcNumRemovedRows(removedSubCells) >= cameraShakeNumRowsLimit) {
                    mEffectManager.StartSmallCameraShake();
                }
                
                mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                mCollapsingFieldAnimation.GoToInactiveState();
                mCollapsingFieldAnimation.ResetBlockAnimations();
                
                if (impactedLevelBombs.IsEmpty()) {
                    RemoveClearedRowsAndPullDownLoosePieces();
                }
            }
        }
    }
    
    if (mState != State::FieldExplosions && mCascadeState == CascadeState::NotCascading) {
        mField.ManageWelds();
    }
    
    NextMove();
}

void GameLogic::DetonateDroppedBomb() {
    GoToFieldExplosionsState();
    
    mComboTextAnimation.Start(ComboTextAnimation::Message::Awesome);

    mEngine.GetAudio().PlaySound(CommonResources::mBombSound);
    
    auto impactedLevelBombs {
        mField.DetectImpactedBombs(CreatePieceBlocks(*mFallingPiece),
                                   mFallingPiece->GetIntPosition())
    };
    
    auto intPieceDetonationPos {mFallingPiece->GetIntPosition() + Pht::IVec2{1, 1}};
    auto pieceDetonationPos {mFallingPiece->GetRenderablePosition() + Pht::Vec2{1.0f, 1.0f}};
    
    if (mFallingPiece->GetPieceType().IsRowBomb()) {
        if (!impactedLevelBombs.IsEmpty()) {
            auto& impactedLevelBomb {impactedLevelBombs.Front()};
            
            switch (impactedLevelBomb.mKind) {
                case BlockKind::Bomb:
                    mFieldExplosionsStates.DetonateLevelBomb(impactedLevelBomb.mPosition);
                    break;
                case BlockKind::RowBomb:
                    mFieldExplosionsStates.DetonateRowBomb(impactedLevelBomb.mPosition);
                    break;
                default:
                    break;
            }
        }
        
        mFieldExplosionsStates.DetonateRowBomb(intPieceDetonationPos, pieceDetonationPos);
    } else {
        if (!impactedLevelBombs.IsEmpty() && impactedLevelBombs.Front().mKind == BlockKind::Bomb) {
            mFieldExplosionsStates.DetonateBigBomb(intPieceDetonationPos);
        } else {
            mFieldExplosionsStates.DetonateBomb(intPieceDetonationPos, pieceDetonationPos);
        }
        
        if (mBlastRadiusAnimation.IsActive()) {
            mBlastRadiusAnimation.Stop();
        }
    }
}

void GameLogic::DetonateImpactedLevelBombs(const Field::ImpactedBombs& impactedLevelBombs) {
    if (impactedLevelBombs.IsEmpty()) {
        return;
    }
    
    GoToFieldExplosionsState();
    
    for (auto& impactedLevelBomb: impactedLevelBombs) {
        switch (impactedLevelBomb.mKind) {
            case BlockKind::Bomb:
                mFieldExplosionsStates.DetonateLevelBomb(impactedLevelBomb.mPosition);
                break;
            case BlockKind::RowBomb:
                mFieldExplosionsStates.DetonateRowBomb(impactedLevelBomb.mPosition);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void GameLogic::GoToFieldExplosionsState() {
    mState = State::FieldExplosions;
    mField.SetBlocksYPositionAndBounceFlag();
}

void GameLogic::RemoveClearedRowsAndPullDownLoosePieces() {
    mField.RemoveClearedRows();
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    mField.PullDownLoosePieces();
    
    // A second calculation of the lowest visible row and pulling down pieces is needed because the
    // first calculation of lowest visible row could be too high if some piece blocks are remaining
    // inside the spawning area since they have not been pulled down yet at the time of calculating
    // the lowest visible row. The correct lowest visible row can be calculated after those piece
    // blocks have been pulled down.
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    mField.PullDownLoosePieces();

    mField.DetectBlocksThatShouldNotBounce();
    
    mCascadeState = CascadeState::Cascading;
}

void GameLogic::RotatePiece(const Pht::TouchEvent& touchEvent) {
    auto& pieceType {mFallingPiece->GetPieceType()};
    
    if (!pieceType.CanRotateAroundZ()) {
        return;
    }
    
    auto newRotation {CalculateNewRotation(touchEvent)};
    
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
        if (!CollisionDetection::IsIllegalTiltedWeldPosition(mField,
                                                             position,
                                                             newRotation,
                                                             pieceType)) {
            mFallingPiece->SetRotation(newRotation);
        }
    }

    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(*mFallingPiece),
                                                mFallingPiece->GetIntPosition());
}

Rotation GameLogic::CalculateNewRotation(const Pht::TouchEvent& touchEvent) {
    auto numRotations {mFallingPiece->GetPieceType().GetNumRotations()};
    auto rotationInt {static_cast<int>(mFallingPiece->GetRotation())};
    
    if (touchEvent.mLocation.x >= mEngine.GetInput().GetScreenInputSize().x / 2.0f) {
        return static_cast<Rotation>((rotationInt + 1) % numRotations);
    }
    
    auto newRotation {rotationInt - 1};
    
    if (newRotation < 0) {
        newRotation += numRotations;
    }
    
    return static_cast<Rotation>(newRotation);
}

void GameLogic::RotatateAndAdjustPosition(Rotation newRotation,
                                          const PieceBlocks& pieceBlocks,
                                          const Pht::IVec2& position,
                                          Direction collisionDirection) {
    switch (collisionDirection) {
        case Direction::Right: {
            auto freeXPos {mField.DetectFreeSpaceLeft(pieceBlocks, position)};
            if (position.x - freeXPos <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedWeldPosition(mField,
                                                                 Pht::IVec2{freeXPos, position.y},
                                                                 newRotation,
                                                                 mFallingPiece->GetPieceType())) {
                mFallingPiece->SetX(freeXPos + halfColumn);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Left: {
            auto freeXPos {mField.DetectFreeSpaceRight(pieceBlocks, position)};
            if (freeXPos - position.x <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedWeldPosition(mField,
                                                                 Pht::IVec2{freeXPos, position.y},
                                                                 newRotation,
                                                                 mFallingPiece->GetPieceType())) {
                mFallingPiece->SetX(freeXPos + halfColumn);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Up: {
            auto freeYPos {mField.DetectFreeSpaceDown(pieceBlocks, position)};
            if (position.y - freeYPos <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedWeldPosition(mField,
                                                                 Pht::IVec2{position.x, freeYPos},
                                                                 newRotation,
                                                                 mFallingPiece->GetPieceType())) {
                mFallingPiece->SetY(freeYPos);
                mFallingPiece->SetRotation(newRotation);
            }
            break;
        }
        case Direction::Down: {
            auto freeYPos {mField.DetectFreeSpaceUp(pieceBlocks, position)};
            if (freeYPos - position.y <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedWeldPosition(mField,
                                                                 Pht::IVec2{position.x, freeYPos},
                                                                 newRotation,
                                                                 mFallingPiece->GetPieceType())) {
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
    if (!IsThereRoomToSwitchPiece() ||
        !mTutorial.IsSwitchPieceAllowed(GetMovesUsedIncludingCurrent())) {

        return;
    }
    
    mFallingPieceSpawnType = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mSelectablePieces[1] = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mSelectablePieces[0] = &mFallingPiece->GetPieceType();
    mFallingPieceSpawnReason = FallingPieceSpawnReason::Switch;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::SwitchPiece;
}

bool GameLogic::IsThereRoomToSwitchPiece() {
    auto& pieceType {*mCurrentMove.mSelectablePieces[1]};
    
    PieceBlocks pieceBlocks {
        pieceType.GetGrid(Rotation::Deg0),
        pieceType.GetGridNumRows(),
        pieceType.GetGridNumColumns()
    };
 
    auto position {CalculateFallingPieceSpawnPos(pieceType, FallingPieceSpawnReason::Switch)};
    
    Pht::IVec2 intPosition {
        static_cast<int>(std::floor(position.x)),
        static_cast<int>(std::floor(position.y))
    };

    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, intPosition, Pht::IVec2{0, 0}, false);

    return collisionResult.mIsCollision == IsCollision::No;
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
        Pht::IVec2 ghostPiecePosition {mFallingPiece->GetIntPosition().x, mGhostPieceRow};
        auto blastRadiusKind {CalculateBlastRadiusKind(ghostPiecePosition)};
        
        if (blastRadiusKind != mBlastRadiusAnimation.GetActiveKind()) {
            mBlastRadiusAnimation.Start(blastRadiusKind);
        }
    }
}

int GameLogic::GetGhostPieceRow() const {
    return mGhostPieceRow;
}

bool GameLogic::IsInFieldExplosionsState() const {
    return mState == State::FieldExplosions;
}

void GameLogic::StartBlastRadiusAnimationAtGhostPiece() {
    Pht::IVec2 ghostPiecePosition {mFallingPiece->GetIntPosition().x, mGhostPieceRow};
    mBlastRadiusAnimation.Start(CalculateBlastRadiusKind(ghostPiecePosition));

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
    mBlastRadiusAnimation.Start(CalculateBlastRadiusKind(position));
    
    Pht::Vec2 blastRadiusAnimationPos {
        static_cast<float>(position.x) + 1.0f,
        static_cast<float>(position.y) + 1.0f
    };

    mBlastRadiusAnimation.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StopBlastRadiusAnimation() {
    mBlastRadiusAnimation.Stop();
}

BlastRadiusAnimation::Kind GameLogic::CalculateBlastRadiusKind(const Pht::IVec2& position) {
    auto impactedLevelBombsIfDropped {
        mField.DetectImpactedBombs(CreatePieceBlocks(*mFallingPiece), position)
    };
    
    if (!impactedLevelBombsIfDropped.IsEmpty() &&
        impactedLevelBombsIfDropped.Front().mKind == BlockKind::Bomb) {
        
        return BlastRadiusAnimation::Kind::BigBomb;
    }
    
    return BlastRadiusAnimation::Kind::Bomb;
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
    
    switch (mControlType) {
        case ControlType::Click:
            mClickInputHandler.HandleTouch(touchEvent, GetMovesUsedIncludingCurrent());
            break;
        case ControlType::Gesture:
            mGestureInputHandler.HandleTouch(touchEvent);
            break;
    }
}
