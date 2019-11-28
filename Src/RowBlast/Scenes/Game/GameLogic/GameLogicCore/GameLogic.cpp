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
#include "FlashingBlocksAnimationSystem.hpp"
#include "CollapsingFieldAnimationSystem.hpp"
#include "EffectManager.hpp"
#include "PieceDropParticleEffect.hpp"
#include "PieceTrailParticleEffect.hpp"
#include "BlastArea.hpp"
#include "FallingPieceScaleAnimation.hpp"
#include "Shield.hpp"
#include "ValidAreaAnimation.hpp"
#include "ScoreTexts.hpp"
#include "SmallText.hpp"
#include "GameHudController.hpp"
#include "Tutorial.hpp"
#include "AudioResources.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto maxRotateAdjustment = 2;
    const Pht::IVec2 bombDetonationAreaSize {5, 5};
    constexpr float halfColumn = 0.5f;
    constexpr auto landingNoMovementDurationFalling = 1.0f;
    constexpr auto landingMovementDurationFalling = 4.0f;
    constexpr auto cascadeWaitTime = 0.23f;
    constexpr auto shieldHeight = 6;
    constexpr auto whooshSoundDelay = 0.05f;
    constexpr auto landingSoundDelay = 0.1f;
    constexpr auto numUndos = 1000;

    PieceBlocks CreatePieceBlocks(const FallingPiece& fallingPiece) {
        auto& pieceType = fallingPiece.GetPieceType();
    
        return {
            pieceType.GetGrid(fallingPiece.GetRotation()),
            pieceType.GetGridNumRows(),
            pieceType.GetGridNumColumns()
        };
    }

    PieceBlocks CreatePieceBlocks(const DraggedPiece& draggedPiece) {
        auto& pieceType = draggedPiece.GetPieceType();
        
        return {
            pieceType.GetGrid(draggedPiece.GetRotation()),
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
                     GameScene& gameScene,
                     EffectManager& effectManager,
                     FlyingBlocksAnimation& flyingBlocksAnimation,
                     FlashingBlocksAnimationSystem& flashingBlocksAnimation,
                     CollapsingFieldAnimationSystem& collapsingFieldAnimation,
                     PieceDropParticleEffect& pieceDropParticleEffect,
                     PieceTrailParticleEffect& pieceTrailParticleEffect,
                     BlastArea& blastRadiusAnimation,
                     FallingPieceScaleAnimation& fallingPieceScaleAnimation,
                     Shield& shieldAnimation,
                     ValidAreaAnimation& validAreaAnimation,
                     ScoreTexts& scoreTexts,
                     SmallText& smallTextAnimation,
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
    mPieceTrailParticleEffect {pieceTrailParticleEffect},
    mBlastArea {blastRadiusAnimation},
    mFallingPieceScaleAnimation {fallingPieceScaleAnimation},
    mShield {shieldAnimation},
    mValidAreaAnimation {validAreaAnimation},
    mScoreTexts {scoreTexts},
    mSmallText {smallTextAnimation},
    mGameHudController {gameHudController},
    mScene {gameScene},
    mTutorial {tutorial},
    mSettingsService {settingsService},
    mControlType {settingsService.GetControlType()},
    mFieldGravity {field},
    mFieldExplosionsStates {engine, field, mFieldGravity, effectManager, flyingBlocksAnimation},
    mFallingPieceAnimation {*this, mFallingPieceStorage},
    mDraggedPieceStorage {gameScene},
    mDraggedPieceAnimation {engine, gameScene, *this, mDraggedPieceStorage},
    mScoreManager {engine, *this, smallTextAnimation, effectManager},
    mAi {field},
    mDragInputHandler {engine, *this, gameScene, mDraggedPieceStorage},
    mGestureInputHandler {*this, mFallingPieceStorage},
    mClickInputHandler {engine, field, gameScene, *this, tutorial, mAi},
    mFallingPiece {&mFallingPieceStorage} {
    
    scrollController.SetGameLogic(*this);
}

void GameLogic::Init(const Level& level) {
    mLevel = &level;
    mControlType = mTutorial.IsGestureControlsAllowed() ? mSettingsService.GetControlType() : ControlType::Click;
    mIsSwipeGhostPieceEnabled = mSettingsService.IsGhostPieceEnabled();

    mFieldGravity.Init();
    mFieldExplosionsStates.Init();
    mAi.Init(level);
    mDragInputHandler.Init();
    mGestureInputHandler.Init(level);
    mClickInputHandler.Init(level);
    mScoreManager.Init();
    
    if (mLevel->GetSpeed() > 0.0f) {
        mLandingNoMovementDuration = landingNoMovementDurationFalling;
        mLandingMovementDuration = landingMovementDurationFalling;
    } else {
        mLandingNoMovementDuration = std::numeric_limits<float>::max();
        mLandingMovementDuration = std::numeric_limits<float>::max();
    }
    
    mState = State::LogicUpdate;
    mCascadeState = CascadeState::NotCascading;

    RemoveDraggedPiece();
    RemoveFallingPiece();
    mFallingPieceSpawnReason = FallingPieceSpawnReason::NextMove;
    mFallingPieceSpawnType = nullptr;
    mDraggedPieceIndex = PreviewPieceIndex::None;
    
    mAllValidMoves = nullptr;
    
    mMovesLeft = mLevel->GetNumMoves();
    mMovesUsed = 0;
    
    mCurrentMove = MoveData {};
    auto& nextPieceGenerator = mCurrentMove.mNextPieceGenerator;
    nextPieceGenerator.Init(mLevel->GetPieceTypes(), mLevel->GetPieceSequence());
    mCurrentMove.mSelectablePieces[0] = &nextPieceGenerator.GetNext();
    mCurrentMove.mSelectablePieces[1] = &nextPieceGenerator.GetNext();
    mCurrentMoveTmp = mCurrentMove;
    mPreviousMove = mCurrentMove;
    mShouldUndoMove = false;
    mNumUndosUsed = 0;
    
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
                HandleSettingsChange();
                if (mFallingPieceSpawnReason != FallingPieceSpawnReason::None) {
                    auto result = SpawnFallingPiece(mFallingPieceSpawnReason);
                    mFallingPieceSpawnReason = FallingPieceSpawnReason::None;
                    if (result != Result::None) {
                        return result;
                    }
                }
                if (mShouldUndoMove) {
                    mSmallText.StartUndoingMessage();
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
    
    if (mBlastArea.IsActive()) {
        mBlastArea.Stop();
    }
    
    UpdateLevelProgress();
    mScoreManager.OnSpawnPiece();
    
    if (mNumObjectsLeftToClear == 0) {
        return Result::LevelCompleted;
    }
    
    mScene.GetHud().UnHideAllSelectablePreviewPieces();
    ShowFallingPiece();
    auto& pieceType = CalculatePieceType(fallingPieceSpawnReason);
    auto rotation = CalculateFallingPieceRotation(pieceType, fallingPieceSpawnReason);
    auto spawnPosition = CalculateFallingPieceSpawnPos(pieceType, fallingPieceSpawnReason);
    mFallingPiece->Spawn(pieceType, spawnPosition, rotation, mLevel->GetSpeed());
    
    ManageMoveHistory(fallingPieceSpawnReason);
    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(*mFallingPiece),
                                                mFallingPiece->GetIntPosition());
    if (mGhostPieceRow > mFallingPiece->GetPosition().y) {
        RemoveFallingPiece();
        return Result::GameOver;
    }
    
    if (mControlType == ControlType::Gesture && mCurrentMove.mPieceType->IsBomb() &&
        mIsSwipeGhostPieceEnabled &&
        fallingPieceSpawnReason != FallingPieceSpawnReason::BeginDraggingPiece) {

        StartBlastAreaAtGhostPiece();
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

const Piece& GameLogic::CalculatePieceType(FallingPieceSpawnReason fallingPieceSpawnReason) {
    if (mFallingPieceSpawnType) {
        if (fallingPieceSpawnReason == FallingPieceSpawnReason::BeginDraggingPiece) {
            auto* tmp = mFallingPieceSpawnType;
            mFallingPieceSpawnType = nullptr;
            return *tmp;
        }
        
        mCurrentMove.mPieceType = mFallingPieceSpawnType;
        mFallingPieceSpawnType = nullptr;
    } else {
        switch (mDraggedPieceIndex) {
            case PreviewPieceIndex::None:
                mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[0];
                mCurrentMove.mSelectablePieces[0] = mCurrentMove.mSelectablePieces[1];
                mCurrentMove.mSelectablePieces[1] = &mCurrentMove.mNextPieceGenerator.GetNext();
                mCurrentMove.mPreviewPieceRotations = PieceRotations {};
                mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::NextPieceAndSwitch;
                break;
            case PreviewPieceIndex::Active:
                SetPreviewPiece(PreviewPieceIndex::Active,
                                &mCurrentMove.mNextPieceGenerator.GetNext(),
                                Rotation::Deg0,
                                Rotation::Deg0);
                mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::NextPieceAndRefillActive;
                break;
            case PreviewPieceIndex::Selectable0:
                SetPreviewPiece(PreviewPieceIndex::Selectable0,
                                &mCurrentMove.mNextPieceGenerator.GetNext(),
                                Rotation::Deg0,
                                Rotation::Deg0);
                mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::NextPieceAndRefillSelectable0;
                break;
            case PreviewPieceIndex::Selectable1:
                SetPreviewPiece(PreviewPieceIndex::Selectable1,
                                &mCurrentMove.mNextPieceGenerator.GetNext(),
                                Rotation::Deg0,
                                Rotation::Deg0);
                mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::NextPieceAndRefillSelectable1;
                break;
        }
        
        mDraggedPieceIndex = PreviewPieceIndex::None;
    }
    
    if (fallingPieceSpawnReason == FallingPieceSpawnReason::UndoMove || mShouldUndoMove) {
        mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
    }
    
    return *mCurrentMove.mPieceType;
}

void GameLogic::SetPreviewPiece(PreviewPieceIndex previewPieceIndex,
                                const Piece* pieceType,
                                Rotation rotation,
                                Rotation hudRotation) {
    auto& pieceRotations = mCurrentMove.mPreviewPieceRotations;
    switch (previewPieceIndex) {
        case PreviewPieceIndex::Active:
            mCurrentMove.mPieceType = pieceType;
            pieceRotations.mRotations.mActive = rotation;
            pieceRotations.mHudRotations.mActive = hudRotation;
            break;
        case PreviewPieceIndex::Selectable0:
            mCurrentMove.mSelectablePieces[0] = pieceType;
            pieceRotations.mRotations.mSelectable0 = rotation;
            pieceRotations.mHudRotations.mSelectable0 = hudRotation;
            break;
        case PreviewPieceIndex::Selectable1:
            mCurrentMove.mSelectablePieces[1] = pieceType;
            pieceRotations.mRotations.mSelectable1 = rotation;
            pieceRotations.mHudRotations.mSelectable1 = hudRotation;
            break;
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}

const Piece* GameLogic::GetPieceType() const {
    if (mFallingPiece || mFallingPieceSpawnReason == FallingPieceSpawnReason::UndoMove ||
        mDraggedPieceIndex != PreviewPieceIndex::None) {

        return mCurrentMove.mPieceType;
    }
    
    return nullptr;
}

const TwoPieces& GameLogic::GetSelectablePieces() const {
    return mCurrentMove.mSelectablePieces;
}

const PreviewPieceRotations& GameLogic::GetPreviewPieceRotations() const {
    return mCurrentMove.mPreviewPieceRotations.mRotations;
}

const PreviewPieceRotations& GameLogic::GetPreviewPieceHudRotations() const {
    return mCurrentMove.mPreviewPieceRotations.mHudRotations;
}

void GameLogic::ManageMoveHistory(FallingPieceSpawnReason fallingPieceSpawnReason) {
    switch (fallingPieceSpawnReason) {
        case FallingPieceSpawnReason::NextMove:
            mCurrentMove.mId = mFallingPiece->GetId();
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
            mCurrentMove.mId = mFallingPiece->GetId();
            mCurrentMoveTmp = mCurrentMove;
            mPreviousMove = mCurrentMove;
            mScoreManager.OnUndoMove();
            mTutorial.OnNewMove(GetMovesUsedIncludingCurrent());
            break;
        case FallingPieceSpawnReason::Switch:
            mTutorial.OnSwitchPiece(GetMovesUsedIncludingCurrent(), mFallingPiece->GetPieceType());
            break;
        case FallingPieceSpawnReason::BeginDraggingPiece:
        case FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece:
        case FallingPieceSpawnReason::None:
            break;
    }
}

void GameLogic::RemoveFallingPiece() {
    mFallingPiece = nullptr;
}

void GameLogic::ShowFallingPiece() {
    mFallingPiece = &mFallingPieceStorage;
}

void GameLogic::RemoveDraggedPiece() {
    mDraggedPiece = nullptr;
}

void GameLogic::ShowDraggedPiece() {
    mDraggedPiece = &mDraggedPieceStorage;
}

Pht::Vec2 GameLogic::CalculateFallingPieceSpawnPos(const Piece& pieceType,
                                                   FallingPieceSpawnReason fallingPieceSpawnReason) {
    auto rotation = pieceType.GetSpawnRotation();
    auto startXPos = mField.GetNumColumns() / 2 - pieceType.GetGridNumColumns() / 2;
    
    if ((fallingPieceSpawnReason == FallingPieceSpawnReason::Switch ||
         fallingPieceSpawnReason == FallingPieceSpawnReason::BeginDraggingPiece ||
         fallingPieceSpawnReason == FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece) &&
        mLevel->GetSpeed() > 0.0f) {
            
        auto pieceNumEmptyBottompRows = pieceType.GetDimensions(rotation).mYmin;
        
        auto previousPieceNumEmptyBottomRows =
            mFallingPiece->GetPieceType().GetDimensions(rotation).mYmin;
        
        auto yAdjust =
            static_cast<float>(previousPieceNumEmptyBottomRows - pieceNumEmptyBottompRows);
        
        return Pht::Vec2 {startXPos + halfColumn, mFallingPiece->GetPosition().y + yAdjust};
    }
    
    auto topRowInScreen =
        static_cast<int>(mScrollController.GetLowestVisibleRow()) + mField.GetNumRowsInOneScreen() - 1;
    
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
    
    auto& pieceDimensions = pieceType.GetDimensions(rotation);
    auto pieceNumEmptyTopRows = pieceType.GetGridNumRows() - pieceDimensions.mYmax - 1;
    auto yPosAdjustment = pieceType.GetSpawnYPositionAdjustment();
    auto desiredUpperPos = topRowInScreen - 3 + pieceNumEmptyTopRows + yPosAdjustment;
    if (mLevel->GetSpeed() > 0.0f) {
        ++desiredUpperPos;
    }

    auto startYPos = desiredUpperPos - pieceType.GetGridNumRows() + 1;
    return Pht::Vec2 {startXPos + halfColumn, static_cast<float>(startYPos)};
}

Rotation GameLogic::CalculateFallingPieceRotation(const Piece& pieceType,
                                                  FallingPieceSpawnReason fallingPieceSpawnReason) {
    switch (fallingPieceSpawnReason) {
        case FallingPieceSpawnReason::BeginDraggingPiece:
            return mDraggedPiece->GetRotation();
        case FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece:
            return mCurrentMove.mPreviewPieceRotations.mRotations.mActive;
        default:
            return pieceType.GetSpawnRotation();
    }
}

void GameLogic::HandleCascading() {
    switch (mCascadeState) {
        case CascadeState::Cascading:
            if (mField.AnyFilledRows()) {
                mCascadeWaitTime = 0.0f;
                mCascadeState = CascadeState::WaitingToClearRows;
            } else {
                mCascadeState = CascadeState::NotCascading;
                mField.ManageWelds();
            }
            break;
        case CascadeState::WaitingToClearRows:
            mCascadeWaitTime += mEngine.GetLastFrameSeconds();
            if ((mCascadeWaitTime > cascadeWaitTime || mCollapsingFieldAnimation.IsInactive())
                && !mScrollController.IsScrolling()) {

                auto removedSubCells = mField.ClearFilledRows();
                mScoreManager.OnClearedFilledRows(removedSubCells);
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
        RemoveClearedRowsAndPullDownLoosePieces();
    }
    
    UpdateLevelProgress();
}

void GameLogic::HandleSettingsChange() {
    if (mFallingPiece == nullptr) {
        return;
    }
    
    auto settingsChanged = false;
    
    if (mSettingsService.GetControlType() != mControlType && mTutorial.IsGestureControlsAllowed()) {
        mCurrentMove.mPreviewPieceRotations = PieceRotations {};
        mCurrentMoveTmp.mPreviewPieceRotations = PieceRotations {};
        mPreviousMove.mPreviewPieceRotations = PieceRotations {};

        if (mSettingsService.GetControlType() == ControlType::Click) {
            mClickInputHandler.CalculateMoves(*mFallingPiece, GetMovesUsedIncludingCurrent() - 1);
            mClickInputHandler.CreateNewSetOfVisibleMoves();
        }
        
        mControlType = mSettingsService.GetControlType();
        settingsChanged = true;
    }
    
    if (mSettingsService.IsGhostPieceEnabled() != mIsSwipeGhostPieceEnabled) {
        mIsSwipeGhostPieceEnabled = mSettingsService.IsGhostPieceEnabled();
        settingsChanged = true;
    }
    
    if (settingsChanged) {
        ManageBlastArea();
    }
}

void GameLogic::ManageBlastArea() {
    if (mIsSwipeGhostPieceEnabled && mControlType == ControlType::Gesture) {
        if (mCurrentMove.mPieceType && mCurrentMove.mPieceType->IsBomb()) {
            StartBlastAreaAtGhostPiece();
        }
    } else {
        if (mBlastArea.IsActive()) {
            mBlastArea.Stop();
        }
    }
}

void GameLogic::UpdateLevelProgress() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mNumObjectsLeftToClear = mField.CalculateNumLevelBlocks();
            break;
        case Level::Objective::BringDownTheAsteroid: {
            auto asteroidRow = mField.CalculateAsteroidRow();
            assert(asteroidRow.HasValue());
            if (asteroidRow.GetValue() == 0) {
                mNumObjectsLeftToClear = 0;
            } else {
                mNumObjectsLeftToClear = 1;
            }
            break;
        }
        case Level::Objective::Build: {
            mNumObjectsLeftToClear = mField.CalculateNumEmptyBlueprintSlots();
            break;
        }
    }
}

void GameLogic::NextMove() {
    mCurrentMoveTmp.mPieceType = mCurrentMove.mPieceType;
    mCurrentMoveTmp.mSelectablePieces = mCurrentMove.mSelectablePieces;
    
    RemoveFallingPiece();
    mFallingPieceSpawnReason = FallingPieceSpawnReason::NextMove;
    
    if (mDraggedPieceIndex == PreviewPieceIndex::None) {
        mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::RemoveActivePiece;
    }
    
    --mMovesLeft;
    UpdateLevelProgress();
}

bool GameLogic::IsUndoMovePossible() const {
    return mCurrentMove.mId != mPreviousMove.mId && mMovesUsed > 1 && mNumUndosUsed < numUndos &&
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
    mDraggedPieceIndex = PreviewPieceIndex::None;
    RemoveFallingPiece();
    ++mMovesLeft;
    --mMovesUsed;
    ++mNumUndosUsed;
    UpdateLevelProgress();
    
    mScene.GetHud().UnHideAndShowAllSelectablePreviewPieces();
}

int GameLogic::GetMovesUsedIncludingCurrent() const {
    return mMovesUsed;
}

void GameLogic::IncreaseScore(int points, const Pht::Vec2& scoreTextPosition) {
    mCurrentMove.mScore += points;
    mScoreTexts.Start(points, scoreTextPosition);
}

int GameLogic::CalculateFinalScore() {
    auto extraPoints = mScoreManager.CalculateExtraPoints(GetScore(),
                                                          mMovesLeft,
                                                          mLevel->GetNumMoves());
    IncreaseScore(extraPoints, {-20.0f, -20.0f});
    return GetScore();
}

void GameLogic::UpdateFallingPieceYpos() {
    auto newYPosition =
        mFallingPiece->GetPosition().y - mFallingPiece->GetSpeed() * mEngine.GetLastFrameSeconds();

    switch (mFallingPiece->GetState()) {
        case FallingPiece::State::Falling:
        case FallingPiece::State::BeingDraggedDown:
            if (newYPosition < mGhostPieceRow) {
                mFallingPiece->GoToLandingState(mGhostPieceRow);
            } else {
                mFallingPiece->SetY(newYPosition);
                if (mFallingPiece->GetPreviousIntY() != static_cast<int>(newYPosition)) {
                    if (mControlType == ControlType::Click) {
                        mClickInputHandler.UpdateMoves(*mFallingPiece,
                                                       GetMovesUsedIncludingCurrent() - 1);
                    } else {
                        auto& validMoves = mAi.FindValidMoves(*mFallingPiece,
                                                              GetMovesUsedIncludingCurrent() - 1);
                        mAllValidMoves = &validMoves.mMoves;
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
                    if (mDraggedPiece) {
                        CancelDraggingBecausePieceLands();
                    }
                    LandFallingPiece(false);
                }
            }
            break;
    }
}

void GameLogic::DropFallingPiece() {
    bool finalMovementWasADrop {mFallingPiece->GetPosition().y > mGhostPieceRow + 1};
    mFallingPiece->SetY(mGhostPieceRow);
    
    if (finalMovementWasADrop) {
        mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::DropWhoosh));
    }
    
    LandFallingPiece(finalMovementWasADrop);
}

void GameLogic::SelectMove(const Move& move) {
    mTutorial.OnSelectMove(GetMovesUsedIncludingCurrent());
    mEngine.GetAudio().PlaySoundWithDelay(static_cast<Pht::AudioResourceId>(SoundId::DropWhoosh),
                                          whooshSoundDelay);
    mFallingPieceAnimation.Start(*move.mLastMovement);
}

void GameLogic::OnFallingPieceAnimationFinished(bool finalMovementWasADrop) {
    LandFallingPiece(finalMovementWasADrop);
}

void GameLogic::LandFallingPiece(bool finalMovementWasADrop) {
    mField.SaveState();
    
    auto& pieceType = mFallingPiece->GetPieceType();
    if (IsBomb(pieceType)) {
        if (finalMovementWasADrop) {
            mPieceDropParticleEffect.StartEffect(*mFallingPiece);
        }

        DetonateDroppedBomb();
    } else {
        auto impactedLevelBombs =
            mField.DetectImpactedBombs(CreatePieceBlocks(*mFallingPiece),
                                       mFallingPiece->GetIntPosition());
        auto startBounceAnimation = false;
        
        if (finalMovementWasADrop) {
            startBounceAnimation = true;
            mCollapsingFieldAnimation.GoToBlocksBouncingState();
            mPieceTrailParticleEffect.StartEffect(*mFallingPiece);
        }
        
        mFlashingBlocksAnimation.ResetFlashingBlockAnimations();
        mField.LandFallingPiece(*mFallingPiece, startBounceAnimation);
        mFlashingBlocksAnimation.Start(pieceType.GetColor());
        DetonateImpactedLevelBombs(impactedLevelBombs);
        
        switch (mLevel->GetObjective()) {
            case Level::Objective::Clear:
            case Level::Objective::BringDownTheAsteroid: {
                auto removedSubCells = mField.ClearFilledRows();
                if (removedSubCells.IsEmpty()) {
                    mScoreManager.OnClearedNoFilledRows();
                } else {
                    mScoreManager.OnClearedFilledRows(removedSubCells, mFallingPiece->GetId());
                    mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
                    mCollapsingFieldAnimation.GoToInactiveState();
                    mCollapsingFieldAnimation.ResetBlockAnimations();
                    if (impactedLevelBombs.IsEmpty()) {
                        RemoveClearedRowsAndPullDownLoosePieces();
                    }
                }
                break;
            }
            case Level::Objective::Build: {
                auto slotsCoveredByPiece = mField.CalculatePieceFilledSlots(*mFallingPiece);
                mScoreManager.OnFilledSlots(slotsCoveredByPiece);
                break;
            }
        }
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
    
    auto impactedLevelBombs =
        mField.DetectImpactedBombs(CreatePieceBlocks(*mFallingPiece),
                                   mFallingPiece->GetIntPosition());

    auto intPieceDetonationPos = mFallingPiece->GetIntPosition() + Pht::IVec2{1, 1};
    auto pieceDetonationPos = mFallingPiece->GetRenderablePosition() + Pht::Vec2{1.0f, 1.0f};
    
    if (mFallingPiece->GetPieceType().IsRowBomb()) {
        if (!impactedLevelBombs.IsEmpty()) {
            auto& impactedLevelBomb = impactedLevelBombs.Front();
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
        
        if (mBlastArea.IsActive()) {
            mBlastArea.Stop();
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

void GameLogic::RemoveClearedRowsAndPullDownLoosePieces(bool doBounceCalculations,
                                                        bool resetIsPulledDownFlags) {
    mField.RemoveClearedRows();
    
    if (resetIsPulledDownFlags) {
        mFieldGravity.ResetIsPulledDownFlags();
    }
    
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
    
    mScoreManager.GoToCascadingState();
    mCascadeState = CascadeState::Cascading;
}

void GameLogic::PullDownLoosePiecesClearObjective() {
    // Several calculations of the lowest visible row and pulling down pieces may be needed because
    // the first calculations of lowest visible row could be too high if some piece blocks are
    // remaining inside the spawning area since they have not been pulled down yet at the time of
    // calculating the lowest visible row. The correct lowest visible row can be calculated after
    // those piece blocks have been pulled down.
    for (;;) {
        mField.SetLowestVisibleRow(mScrollController.CalculatePreferredLowestVisibleRow());
        mFieldGravity.PullDownLoosePieces();
        
        if (!mFieldGravity.AnyPiecesPulledDown()) {
            break;
        }
    }
}

void GameLogic::PullDownLoosePiecesAsteroidObjective() {
    // We first need to pull everything down as far as possible in order to calculate the lowest
    // visible row. This is because the lowest visible row is calculated based on the asteroid
    // position and the asteroid needs to be pulled down as far as possible to know its proper
    // position.
    mField.SaveInTempGrid();
    mField.SetLowestVisibleRow(0);
    mFieldGravity.PullDownLoosePieces();
    
    auto lowestVisibleRow = mScrollController.CalculatePreferredLowestVisibleRow();
    
    // Now that we have the correct lowest visible row, we can restore the field and do the proper
    // pull down of the pieces.
    mField.RestoreFromTempGrid();
    mField.SetLowestVisibleRow(lowestVisibleRow);
    mFieldGravity.PullDownLoosePieces();
}

void GameLogic::PlayLandPieceSound() {
    auto& audio = mEngine.GetAudio();
    auto landingSoundResourceId = static_cast<Pht::AudioResourceId>(SoundId::LandPiece);

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
    auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
    Pht::IVec2 areaPosition {0, lowestVisibleRow + Shield::shieldRelativeYPosition};
    Pht::IVec2 areaSize {mField.GetNumColumns(), shieldHeight};
    auto removeCorners = true;
    auto removedSubCells = mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners);

    if (removedSubCells.Size() > 0) {
        mFlyingBlocksAnimation.AddBlocksRemovedByTheShield(removedSubCells, mField.GetNumColumns());
        mShield.StartFlash();
        PlayClearBlocksSound(mEngine);
        
        if (mState != State::FieldExplosions) {
            // It is not safe to pull down pieces while in FieldExplosions state.
            auto doBounceCalculations = false;
            auto resetIsPulledDownFlags = false;
            RemoveClearedRowsAndPullDownLoosePieces(doBounceCalculations, resetIsPulledDownFlags);
            
            // Blocks that have just landed are in bouncing state. Some of those recently landed
            // blocks that belonged to a piece that was partly removed by the shield are still
            // (wrongly) in bouncing state even though they will fall further since the blocks that
            // connected them to the ground are removed. Fix this by transitioning those blocks to
            // the falling state.
            mCollapsingFieldAnimation.TransitionWronglyBouncingBlocksToFalling();
        }
    }
}

void GameLogic::RotatePreviewPiece(PreviewPieceIndex previewPieceIndex) {
    switch (previewPieceIndex) {
        case PreviewPieceIndex::Active:
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mRotations.mActive,
                               mCurrentMove.mPieceType,
                               Pht::Optional<int> {});
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mHudRotations.mActive,
                               mCurrentMove.mPieceType,
                               4);
            break;
        case PreviewPieceIndex::Selectable0:
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mRotations.mSelectable0,
                               mCurrentMove.mSelectablePieces[0],
                               Pht::Optional<int> {});
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mHudRotations.mSelectable0,
                               mCurrentMove.mSelectablePieces[0],
                               4);
            break;
        case PreviewPieceIndex::Selectable1:
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mRotations.mSelectable1,
                               mCurrentMove.mSelectablePieces[1],
                               Pht::Optional<int> {});
            RotatePreviewPiece(mCurrentMove.mPreviewPieceRotations.mHudRotations.mSelectable1,
                               mCurrentMove.mSelectablePieces[1],
                               4);
            break;
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}

void GameLogic::RotatePreviewPieces() {
    if (!mSettingsService.IsRotateAllPiecesEnabled()) {
        return;
    }

    RotatePreviewPieces(mCurrentMove.mPreviewPieceRotations.mRotations, Pht::Optional<int> {});
    RotatePreviewPieces(mCurrentMove.mPreviewPieceRotations.mHudRotations, 4);
}

void GameLogic::RotatePreviewPieces(PreviewPieceRotations& previewPieceRotations,
                                    Pht::Optional<int> numRotations) {
    RotatePreviewPiece(previewPieceRotations.mActive, mCurrentMove.mPieceType, numRotations);
    RotatePreviewPiece(previewPieceRotations.mSelectable0, mCurrentMove.mSelectablePieces[0],
                       numRotations);
    RotatePreviewPiece(previewPieceRotations.mSelectable1, mCurrentMove.mSelectablePieces[1],
                       numRotations);
}

void GameLogic::RotatePreviewPiece(Rotation& previewPieceRotation,
                                   const Piece* pieceType,
                                   Pht::Optional<int> numRotationsOptional) {
    if (pieceType == nullptr) {
        return;
    }

    auto numRotations = numRotationsOptional.HasValue() ? numRotationsOptional.GetValue() :
                        pieceType->GetNumRotations();
    auto rotationInt = static_cast<int>(previewPieceRotation);
    previewPieceRotation = static_cast<Rotation>((rotationInt + 1) % numRotations);
}

void GameLogic::RotatePiece(const Pht::TouchEvent& touchEvent) {
    auto& pieceType = mFallingPiece->GetPieceType();
    if (!pieceType.CanRotateAroundZ()) {
        return;
    }
    
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::RotateWhoosh));
    
    auto newRotation = CalculateNewRotation(touchEvent);
    
    PieceBlocks pieceBlocks {
        pieceType.GetGrid(newRotation),
        pieceType.GetGridNumRows(),
        pieceType.GetGridNumColumns()
    };
    
    auto position = mFallingPiece->GetIntPosition();
    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, position, Pht::IVec2{0, 0}, false);
    if (collisionResult.mIsCollision == IsCollision::Yes) {
        auto collisionDirection =
            CollisionDetection::CalculateCollisionDirection(collisionResult.mCollisionPoints,
                                                            pieceBlocks,
                                                            newRotation,
                                                            position,
                                                            mField);
        
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
    auto numRotations = mFallingPiece->GetPieceType().GetNumRotations();
    auto rotationInt = static_cast<int>(mFallingPiece->GetRotation());
    
    if (touchEvent.mLocation.x >= mEngine.GetInput().GetScreenInputSize().x / 2.0f) {
        return static_cast<Rotation>((rotationInt + 1) % numRotations);
    }
    
    auto newRotation = rotationInt - 1;
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
            auto freeXPos = mField.DetectFreeSpaceLeft(pieceBlocks, position);
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
            auto freeXPos = mField.DetectFreeSpaceRight(pieceBlocks, position);
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
            auto freeYPos = mField.DetectFreeSpaceDown(pieceBlocks, position);
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
            auto freeYPos = mField.DetectFreeSpaceUp(pieceBlocks, position);
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
    auto& nextPieceType = *mCurrentMove.mSelectablePieces[0];
    if (!IsThereRoomToSwitchPiece(nextPieceType, nextPieceType.GetSpawnRotation()) ||
        !mTutorial.IsSwitchPieceAllowed()) {

        mSmallText.StartNoRoomMessage();
        return;
    }
    
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::SwitchPiece));
    
    auto* previousActivePieceType = mCurrentMove.mPieceType;
    
    mFallingPieceSpawnType = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mSelectablePieces[0] = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mSelectablePieces[1] = previousActivePieceType;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::SwitchPiece;
    mCurrentMove.mPreviewPieceRotations = PieceRotations {};
    
    SpawnFallingPiece(FallingPieceSpawnReason::Switch);
}

bool GameLogic::IsThereRoomToSwitchPiece(const Piece& pieceType, Rotation rotation) {
    PieceBlocks pieceBlocks {
        pieceType.GetGrid(rotation),
        pieceType.GetGridNumRows(),
        pieceType.GetGridNumColumns()
    };
 
    auto position = CalculateFallingPieceSpawnPos(pieceType, FallingPieceSpawnReason::Switch);

    Pht::IVec2 intPosition {
        static_cast<int>(std::floor(position.x)),
        static_cast<int>(std::floor(position.y))
    };

    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, intPosition, Pht::IVec2{0, 0}, false);

    return collisionResult.mIsCollision == IsCollision::No;
}

void GameLogic::SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) {
    auto pieceBlocks = CreatePieceBlocks(*mFallingPiece);

    if (fallingPieceNewX - mFallingPiece->GetPosition().x > 0.0f) {
        auto collisionRight =
            mField.DetectCollisionRight(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn;
        
        mFallingPiece->SetX(std::fmin(collisionRight, fallingPieceNewX));
    } else {
        auto collisionLeft =
            mField.DetectCollisionLeft(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn;
        
        mFallingPiece->SetX(std::fmax(collisionLeft, fallingPieceNewX));
    }

    mGhostPieceRow = mField.DetectCollisionDown(pieceBlocks, mFallingPiece->GetIntPosition());
    
    if (mBlastArea.IsActive()) {
        SetBlastAreaPositionAtGhostPiece();
        Pht::IVec2 ghostPiecePosition {mFallingPiece->GetIntPosition().x, mGhostPieceRow};
        auto blastRadiusKind = CalculateBlastRadiusKind(CreatePieceBlocks(*mFallingPiece),
                                                        ghostPiecePosition);
        if (blastRadiusKind != mBlastArea.GetActiveKind()) {
            mBlastArea.Start(blastRadiusKind);
        }
    }
}

int GameLogic::GetGhostPieceRow() const {
    return mGhostPieceRow;
}

bool GameLogic::IsInFieldExplosionsState() const {
    return mState == State::FieldExplosions;
}

void GameLogic::StartBlastAreaAtGhostPiece() {
    Pht::IVec2 ghostPiecePosition {mFallingPiece->GetIntPosition().x, mGhostPieceRow};
    mBlastArea.Start(CalculateBlastRadiusKind(CreatePieceBlocks(*mFallingPiece),
                                              ghostPiecePosition));
    SetBlastAreaPositionAtGhostPiece();
}

void GameLogic::SetBlastAreaPositionAtGhostPiece() {
    Pht::Vec2 blastRadiusAnimationPos {
        mFallingPiece->GetRenderablePosition().x + 1.0f,
        static_cast<float>(mGhostPieceRow) + 1.0f
    };

    mBlastArea.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StartBlastArea(const Pht::IVec2& position) {
    mBlastArea.Start(CalculateBlastRadiusKind(CreatePieceBlocks(*mFallingPiece), position));
    
    Pht::Vec2 blastRadiusAnimationPos {
        static_cast<float>(position.x) + 1.0f,
        static_cast<float>(position.y) + 1.0f
    };

    mBlastArea.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StopBlastArea() {
    mBlastArea.Stop();
}

BlastArea::Kind GameLogic::CalculateBlastRadiusKind(const PieceBlocks& pieceBlocks,
                                                    const Pht::IVec2& position) {
    auto impactedLevelBombsIfDropped = mField.DetectImpactedBombs(pieceBlocks, position);
    if (!impactedLevelBombsIfDropped.IsEmpty() &&
        impactedLevelBombsIfDropped.Front().mKind == BlockKind::Bomb) {
        
        return BlastArea::Kind::BigBomb;
    }
    
    return BlastArea::Kind::Bomb;
}

bool GameLogic::BeginDraggingPiece(PreviewPieceIndex draggedPieceIndex) {
    if (draggedPieceIndex == PreviewPieceIndex::None) {
        return false;
    }
    
    ShowDraggedPiece();
    auto& pieceType = mDraggedPiece->GetPieceType();
    if (!IsThereRoomToSwitchPiece(pieceType, mDraggedPiece->GetRotation())) {
        mSmallText.StartNoRoomMessage();
        RemoveDraggedPiece();
        return false;
    }
    
    mDraggedPieceIndex = draggedPieceIndex;
    mFallingPieceSpawnType = &pieceType;
    if (SpawnFallingPiece(FallingPieceSpawnReason::BeginDraggingPiece) != Result::None) {
        RemoveDraggedPiece();
        mDraggedPieceIndex = PreviewPieceIndex::None;
        return false;
    }
    
    mScene.GetHud().HidePreviewPiece(draggedPieceIndex);
    
    auto& validMoves = mAi.FindValidMoves(*mFallingPiece,
                                          GetMovesUsedIncludingCurrent() - 1);
    mAllValidMoves = &validMoves.mMoves;
    
    mValidAreaAnimation.Start(validMoves.mMoves, pieceType, mDraggedPiece->GetRotation());
    mDraggedPieceAnimation.StartGoUpAnimation();
    UpdateDraggedGhostPieceRowAndBlastArea();
    return true;
}

void GameLogic::OnDraggedPieceMoved() {
    auto pieceBlocks = CreatePieceBlocks(*mFallingPiece);
    auto fallingPieceNewX =
        static_cast<float>(mDraggedPiece->GetFieldGridPosition().x) + halfColumn;

    if (fallingPieceNewX - mFallingPiece->GetPosition().x > 0.0f) {
        auto collisionRight =
            mField.DetectCollisionRight(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn;
        
        mFallingPiece->SetX(std::fmin(collisionRight, fallingPieceNewX));
    } else {
        auto collisionLeft =
            mField.DetectCollisionLeft(pieceBlocks, mFallingPiece->GetIntPosition()) +
            halfColumn;
        
        mFallingPiece->SetX(std::fmax(collisionLeft, fallingPieceNewX));
    }

    UpdateDraggedGhostPieceRowAndBlastArea();
}

void GameLogic::StopDraggingPiece() {
    auto ghostPieceRow = 0;
    if (auto* move = GetValidMoveBelowDraggedPiece(ghostPieceRow)) {
        SelectMove(*move);
        mValidAreaAnimation.Stop();
        RemoveDraggedPiece();
    } else {
        mDraggedPieceAnimation.StartGoBackAnimation(mDraggedPieceIndex);
    }
}

void GameLogic::CancelDraggingPiece() {
    mFallingPieceSpawnType = mCurrentMove.mPieceType;
    SpawnFallingPiece(FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece);
    mScene.GetHud().ShowPreviewPiece(mDraggedPieceIndex);
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mValidAreaAnimation.Stop();
    RemoveDraggedPiece();
}

void GameLogic::CancelDraggingBecausePieceLands() {
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mValidAreaAnimation.Stop();
    RemoveDraggedPiece();
    mDragInputHandler.EndDrag();
}

void GameLogic::OnDraggedPieceAnimationFinished() {
    CancelDraggingPiece();
}

void GameLogic::UpdateDraggedGhostPieceRowAndBlastArea() {
    auto ghostPieceRow = 0;
    if (GetValidMoveBelowDraggedPiece(ghostPieceRow)) {
        mDraggedGhostPieceRow = ghostPieceRow;
        if (mDraggedPiece->GetPieceType().IsBomb()) {
            Pht::IVec2 ghostPiecePosition {
                mDraggedPiece->GetFieldGridPosition().x,
                mDraggedGhostPieceRow.GetValue()
            };
        
            Pht::Vec2 blastRadiusAnimationPos {
                static_cast<float>(ghostPiecePosition.x) + 1.0f,
                static_cast<float>(ghostPiecePosition.y) + 1.0f
            };
            
            mBlastArea.SetPosition(blastRadiusAnimationPos);
            auto blastRadiusKind = CalculateBlastRadiusKind(CreatePieceBlocks(*mDraggedPiece),
                                                            ghostPiecePosition);
            if (mBlastArea.IsActive()) {
                if (blastRadiusKind != mBlastArea.GetActiveKind()) {
                    mBlastArea.Start(blastRadiusKind);
                }
            } else {
                mBlastArea.Start(blastRadiusKind);
            }
        }
    } else {
        mDraggedGhostPieceRow = Pht::Optional<int> {};
        if (mBlastArea.IsActive()) {
            mBlastArea.Stop();
        }
    }
}

const Move* GameLogic::GetValidMoveBelowDraggedPiece(int& ghostPieceRow) {
    auto pieceBlocks = CreatePieceBlocks(*mDraggedPiece);
    auto piecePosition = mDraggedPiece->GetFieldGridPosition();
    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, piecePosition, Pht::IVec2{0, 0}, false);
    if (collisionResult.mIsCollision != IsCollision::Yes) {
        Rotation rotation = mDraggedPiece->GetRotation();
        auto rowIfDropped = mField.DetectCollisionDown(pieceBlocks, piecePosition);
        Pht::IVec2 positionIfDropped {piecePosition.x, rowIfDropped};
        Move moveIfDropped {.mPosition = positionIfDropped, rotation};
        if (auto* validMove = mAllValidMoves->Find(moveIfDropped)) {
            ghostPieceRow = positionIfDropped.y;
            return validMove;
        }

        auto& duplicateMoveCheck = mDraggedPiece->GetPieceType().GetDuplicateMoveCheck(rotation);
        if (duplicateMoveCheck.HasValue()) {
            auto& duplicateMoveCheckValue = duplicateMoveCheck.GetValue();
            
            Move moveIfDropped {
                .mPosition = positionIfDropped + duplicateMoveCheckValue.mRelativePosition,
                .mRotation = duplicateMoveCheckValue.mRotation
            };

            if (auto* validMove = mAllValidMoves->Find(moveIfDropped)) {
                ghostPieceRow = positionIfDropped.y;
                return validMove;
            }
        }
    }
    
    return nullptr;
}

GameLogic::Result GameLogic::HandleInput() {
    auto result = Result::None;
    auto isSwitchButtonEnabled = (mControlType == ControlType::Click);
    auto& input = mEngine.GetInput();
    
    while (input.HasEvents()) {
        auto& event = input.GetNextEvent();
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent = event.GetTouchEvent();
                switch (mGameHudController.OnTouch(touchEvent, isSwitchButtonEnabled)) {
                    case GameHudController::Result::None:
                        ForwardTouchToInputHandler(touchEvent);
                        break;
                    case GameHudController::Result::ClickedSwitch:
                        if (IsInputAllowed()) {
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
    if (!IsInputAllowed()) {
        return;
    }
    
    switch (mControlType) {
        case ControlType::Click:
            mClickInputHandler.HandleTouch(touchEvent, GetMovesUsedIncludingCurrent());
            break;
        case ControlType::Gesture:
            if (mDragInputHandler.HandleTouch(touchEvent) == DragInputHandler::State::Idle) {
                mGestureInputHandler.HandleTouch(touchEvent);
            }
            break;
    }
}

bool GameLogic::IsInputAllowed() const {
    return mFallingPiece &&
           mFallingPieceAnimation.GetState() == FallingPieceAnimation::State::Inactive &&
           (mDraggedPieceAnimation.GetState() == DraggedPieceAnimation::State::Inactive ||
            mDraggedPieceAnimation.GetState() == DraggedPieceAnimation::State::DraggedPieceGoingUp);
}
