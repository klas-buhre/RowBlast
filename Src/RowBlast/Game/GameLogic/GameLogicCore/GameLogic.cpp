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
#include "ShieldAnimation.hpp"
#include "SmallTextAnimation.hpp"
#include "GameHudController.hpp"
#include "Tutorial.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto maxRotateAdjustment {2};
    const Pht::IVec2 bombDetonationAreaSize {5, 5};
    constexpr float halfColumn {0.5f};
    constexpr auto landingNoMovementDurationFalling {1.0f};
    constexpr auto landingMovementDurationFalling {4.0f};
    constexpr auto cascadeWaitTime {0.23f};
    constexpr auto shieldHeight {6};
    constexpr auto whooshSoundDelay {0.05f};
    constexpr auto landingSoundDelay {0.1f};

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
                     ScrollController& scrollController,
                     const GameScene& gameScene,
                     EffectManager& effectManager,
                     FlyingBlocksAnimation& flyingBlocksAnimation,
                     FlashingBlocksAnimation& flashingBlocksAnimation,
                     CollapsingFieldAnimation& collapsingFieldAnimation,
                     PieceDropParticleEffect& pieceDropParticleEffect,
                     BlastRadiusAnimation& blastRadiusAnimation,
                     FallingPieceScaleAnimation& fallingPieceScaleAnimation,
                     ShieldAnimation& shieldAnimation,
                     SmallTextAnimation& smallTextAnimation,
                     GameHudController& gameHudController,
                     Tutorial& tutorial,
                     const SettingsService& settingsService) :
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
    mShieldAnimation {shieldAnimation},
    mSmallTextAnimation {smallTextAnimation},
    mGameHudController {gameHudController},
    mTutorial {tutorial},
    mSettingsService {settingsService},
    mControlType {settingsService.GetControlType()},
    mFieldGravity {field},
    mFieldExplosionsStates {engine, field, mFieldGravity, effectManager, flyingBlocksAnimation},
    mFallingPieceAnimation {*this, mFallingPieceStorage},
    mComboDetector {engine, smallTextAnimation, effectManager},
    mGestureInputHandler {*this, mFallingPieceStorage},
    mClickInputHandler {engine, field, gameScene, *this, tutorial},
    mFallingPiece {&mFallingPieceStorage} {
    
    scrollController.SetGameLogic(*this);
}

void GameLogic::Init(const Level& level) {
    mLevel = &level;
    mControlType = mTutorial.IsGestureControlsAllowed() ? mSettingsService.GetControlType() : ControlType::Click;

    mFieldExplosionsStates.Init();
    mGestureInputHandler.Init(level);
    mClickInputHandler.Init(level);
    mComboDetector.Init();
    
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
    mCurrentMoveTmp = mCurrentMove;
    mPreviousMove = mCurrentMove;
    mShouldUndoMove = false;
    
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
    
    mFallingPieceStorage.ResetBetweenGames();
    
    UpdateLevelProgress();
}

GameLogic::Result GameLogic::Update(bool shouldUpdateLogic, bool shouldUndoMove) {
    if (shouldUndoMove) {
        mShouldUndoMove = true;
    }

    if (mLevel->GetObjective() == Level::Objective::BringDownTheAsteroid &&
        mScrollController.IsScrollingDownInClearMode()) {
        
        RemoveBlocksInsideTheShield();
    }

    switch (mState) {
        case State::LogicUpdate:
            if (shouldUpdateLogic) {
                HandleCascading();
                if (mCascadeState != CascadeState::NotCascading) {
                    return Result::None;
                }
                HandleControlTypeChange();
                if (mFallingPieceSpawnReason != FallingPieceSpawnReason::None) {
                    auto result {SpawnFallingPiece(mFallingPieceSpawnReason)};
                    mFallingPieceSpawnReason = FallingPieceSpawnReason::None;
                    if (result != Result::None) {
                        return result;
                    }
                }
                if (mShouldUndoMove) {
                    mSmallTextAnimation.StartUndoingMessage();
                    UndoMove();
                    return Result::None;
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

GameLogic::Result GameLogic::SpawnFallingPiece(FallingPieceSpawnReason fallingPieceSpawnReason) {
    assert(fallingPieceSpawnReason != FallingPieceSpawnReason::None);
    
    if (mBlastRadiusAnimation.IsActive()) {
        mBlastRadiusAnimation.Stop();
    }
    
    UpdateLevelProgress();
    mComboDetector.OnSpawnPiece();
    
    if (mNumObjectsLeftToClear == 0) {
        return Result::LevelCompleted;
    }
    
    mFallingPiece = &mFallingPieceStorage;
    SetPieceType();
    auto spawnPosition {
        CalculateFallingPieceSpawnPos(*mCurrentMove.mPieceType, fallingPieceSpawnReason)
    };
    mFallingPiece->Spawn(*mCurrentMove.mPieceType, spawnPosition, mLevel->GetSpeed());
    
    ManageMoveHistory(fallingPieceSpawnReason);
    
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

void GameLogic::ManageMoveHistory(FallingPieceSpawnReason fallingPieceSpawnReason) {
    mCurrentMove.mId = mFallingPiece->GetId();
    
    switch (fallingPieceSpawnReason) {
        case FallingPieceSpawnReason::NextMove:
            ++mMovesUsed;
            if (GetMovesUsedIncludingCurrent() > 1) {
                mPreviousMove = mCurrentMoveTmp;
            }
            mCurrentMoveTmp = mCurrentMove;
            if (GetMovesUsedIncludingCurrent() == 1) {
                mPreviousMove = mCurrentMoveTmp;
            }
            mTutorial.OnNewMove(GetMovesUsedIncludingCurrent());
            break;
        case FallingPieceSpawnReason::UndoMove:
            mCurrentMoveTmp = mCurrentMove;
            mPreviousMove = mCurrentMove;
            mComboDetector.OnUndoMove();
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
                mComboDetector.OnClearedFilledRows(removedSubCells);
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
    
    if (mSettingsService.GetControlType() != mControlType && mTutorial.IsGestureControlsAllowed()) {
        switch (mSettingsService.GetControlType()) {
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
        
        mControlType = mSettingsService.GetControlType();
    }
}

void GameLogic::UpdateLevelProgress() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mNumObjectsLeftToClear = mField.CalculateNumLevelBlocks();
            break;
        case Level::Objective::BringDownTheAsteroid: {
            auto asteroidRow {mField.CalculateAsteroidRow()};
            assert(asteroidRow.HasValue());
            if (asteroidRow.GetValue() == 0) {
                mNumObjectsLeftToClear = 0;
            } else {
                mNumObjectsLeftToClear = 1;
            }
            break;
        }
        case Level::Objective::Build:
            mNumObjectsLeftToClear = mField.CalculateNumEmptyBlueprintSlots();
            break;
    }
}

void GameLogic::NextMove() {
    mCurrentMoveTmp.mPieceType = mCurrentMove.mPieceType;
    mCurrentMoveTmp.mSelectablePieces = mCurrentMove.mSelectablePieces;
    
    RemoveFallingPiece();
    
    mFallingPieceSpawnReason = FallingPieceSpawnReason::NextMove;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::RemoveActivePiece;
    --mMovesLeft;
    
    UpdateLevelProgress();
}

bool GameLogic::IsUndoMovePossible() const {
    return mCurrentMove.mId != mPreviousMove.mId && mMovesUsed > 1 &&
           mTutorial.IsUndoMoveAllowed(GetMovesUsedIncludingCurrent());
}

void GameLogic::UndoMove() {
    mShouldUndoMove = false;

    if (!IsUndoMovePossible()) {
        return;
    }
    
    mField.RestorePreviousState();
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    
    mScrollController.GoToIdleState();
    
    mCurrentMove = mPreviousMove;
    mFallingPieceSpawnType = mPreviousMove.mPieceType;
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
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Whoosh));
    LandFallingPiece(finalMovementWasADrop);
}

void GameLogic::SelectMove(const Move& move) {
    mTutorial.OnSelectMove();
    mEngine.GetAudio().PlaySoundWithDelay(static_cast<Pht::AudioResourceId>(SoundId::Whoosh),
                                          whooshSoundDelay);
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
    
    auto clearedAnyFilledRows {false};
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
        
        if (LevelAllowsClearingFilledRows()) {
            auto removedSubCells {mField.ClearFilledRows()};

            if (!removedSubCells.IsEmpty()) {
                clearedAnyFilledRows = true;
                mComboDetector.OnClearedFilledRows(removedSubCells);
                mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                
                mCollapsingFieldAnimation.GoToInactiveState();
                mCollapsingFieldAnimation.ResetBlockAnimations();
                
                if (impactedLevelBombs.IsEmpty()) {
                    RemoveClearedRowsAndPullDownLoosePieces();
                }
            }
        }
    }
    
    if (!clearedAnyFilledRows) {
        mComboDetector.OnClearedNoFilledRows();
    }
    
    if (mLevel->GetObjective() == Level::Objective::BringDownTheAsteroid) {
        RemoveBlocksInsideTheShield();
    }
    
    if (mState != State::FieldExplosions && mCascadeState == CascadeState::NotCascading) {
        mField.ManageWelds();
        PlayLandPieceSound();
    }
    
    NextMove();
}

void GameLogic::DetonateDroppedBomb() {
    GoToFieldExplosionsState();
    
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

void GameLogic::RemoveClearedRowsAndPullDownLoosePieces(bool doBounceCalculations) {
    mField.RemoveClearedRows();
    
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            PullDownLoosePiecesClearObjective();
            break;
        case Level::Objective::BringDownTheAsteroid:
            PullDownLoosePiecesAsteroidObjective();
            break;
        case Level::Objective::Build:
            assert(false);
    }

    if (doBounceCalculations) {
        mFieldGravity.DetectBlocksThatShouldNotBounce();
    }
    
    mComboDetector.GoToCascadingState();
    mCascadeState = CascadeState::Cascading;
}

void GameLogic::PullDownLoosePiecesClearObjective() {
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    mFieldGravity.PullDownLoosePieces();

    // A second calculation of the lowest visible row and pulling down pieces is needed because the
    // first calculation of lowest visible row could be too high if some piece blocks are remaining
    // inside the spawning area since they have not been pulled down yet at the time of calculating
    // the lowest visible row. The correct lowest visible row can be calculated after those piece
    // blocks have been pulled down.
    mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
    mFieldGravity.PullDownLoosePieces();
}

void GameLogic::PullDownLoosePiecesAsteroidObjective() {
    // We first need to pull everything down as far as possible in order to calculate the lowest
    // visible row. This is because the lowest visible row is calculated based on the asteroid
    // position and the asteroid needs to be pulled down as far as possible to know its proper
    // position.
    mField.SaveInTempGrid();
    mField.SetLowestVisibleRow(0);
    mFieldGravity.PullDownLoosePieces();
    
    auto lowestVisibleRow {mScrollController.CalculatePreferredLowestVisibleRow()};
    
    // Now that we have the correct lowest visible row, we can restore the field and do the proper
    // pull down of the pieces.
    mField.RestoreFromTempGrid();
    mField.SetLowestVisibleRow(lowestVisibleRow);
    mFieldGravity.PullDownLoosePieces();
}

bool GameLogic::LevelAllowsClearingFilledRows() const {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
        case Level::Objective::BringDownTheAsteroid:
            return true;
        case Level::Objective::Build:
            return false;
    }
}

void GameLogic::PlayLandPieceSound() {
    auto& audio {mEngine.GetAudio()};
    auto landingSoundResourceId {static_cast<Pht::AudioResourceId>(SoundId::LandPiece)};

    switch (mControlType) {
        case ControlType::Click:
            audio.PlaySound(landingSoundResourceId);
            break;
        case ControlType::Gesture:
            audio.PlaySoundWithDelay(landingSoundResourceId, landingSoundDelay);
            break;
    }
}

void GameLogic::RemoveBlocksInsideTheShield() {
    auto lowestVisibleRow {static_cast<int>(mScrollController.GetLowestVisibleRow())};
    Pht::IVec2 areaPosition {0, lowestVisibleRow + ShieldAnimation::shieldRelativeYPosition};
    Pht::IVec2 areaSize {mField.GetNumColumns(), shieldHeight};
    auto removeCorners {true};
    auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners)};

    if (removedSubCells.Size() > 0) {
        mFlyingBlocksAnimation.AddBlocksRemovedByTheShield(removedSubCells, mField.GetNumColumns());
        mShieldAnimation.StartFlash();
        
        if (mState != State::FieldExplosions) {
            // It is not safe to pull down pieces while in FieldExplosions state.
            auto doBounceCalculations {false};
            RemoveClearedRowsAndPullDownLoosePieces(doBounceCalculations);
            
            // Blocks that have just landed are in bouncing state. Some of those recently landed
            // blocks that belonged to a piece that was partly removed by the shield are still
            // (wrongly) in bouncing state even though they will fall further since the blocks that
            // connected them to the ground are removed. Fix this by transitioning those blocks to
            // the falling state.
            mCollapsingFieldAnimation.TransitionWronglyBouncingBlocksToFalling();
        }
    }
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
    
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ButtonClick));
    
    auto* previousActivePieceType {mCurrentMove.mPieceType};
    
    mFallingPieceSpawnType = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mSelectablePieces[1] = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mSelectablePieces[0] = previousActivePieceType;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::SwitchPiece;
    
    SpawnFallingPiece(FallingPieceSpawnReason::Switch);
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
