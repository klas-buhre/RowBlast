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
#include "FlyingBlocksSystem.hpp"
#include "FlashingBlocksAnimationSystem.hpp"
#include "CollapsingFieldAnimationSystem.hpp"
#include "EffectManager.hpp"
#include "PieceDropParticleEffect.hpp"
#include "PieceTrailParticleEffect.hpp"
#include "PiecePathSystem.hpp"
#include "BlastArea.hpp"
#include "FallingPieceScaleAnimation.hpp"
#include "Shield.hpp"
#include "ScoreTexts.hpp"
#include "MediumText.hpp"
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
    constexpr auto numUndos = 1;

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
                     FlyingBlocksSystem& flyingBlocksSystem,
                     FlashingBlocksAnimationSystem& flashingBlocksAnimation,
                     CollapsingFieldAnimationSystem& collapsingFieldAnimation,
                     PieceDropParticleEffect& pieceDropParticleEffect,
                     PieceTrailParticleEffect& pieceTrailParticleEffect,
                     PiecePathSystem& piecePathSystem,
                     BlastArea& blastRadiusAnimation,
                     FallingPieceScaleAnimation& fallingPieceScaleAnimation,
                     Shield& shieldAnimation,
                     ScoreTexts& scoreTexts,
                     MediumText& mediumTextAnimation,
                     GameHudController& gameHudController,
                     Tutorial& tutorial,
                     const SettingsService& settingsService) :
    mEngine {engine},
    mField {field},
    mScrollController {scrollController},
    mFlyingBlocksSystem {flyingBlocksSystem},
    mFlashingBlocksAnimation {flashingBlocksAnimation},
    mCollapsingFieldAnimation {collapsingFieldAnimation},
    mEffectManager {effectManager},
    mPieceDropParticleEffect {pieceDropParticleEffect},
    mPieceTrailParticleEffect {pieceTrailParticleEffect},
    mPiecePathSystem {piecePathSystem},
    mBlastArea {blastRadiusAnimation},
    mFallingPieceScaleAnimation {fallingPieceScaleAnimation},
    mShield {shieldAnimation},
    mScoreTexts {scoreTexts},
    mMediumText {mediumTextAnimation},
    mGameHudController {gameHudController},
    mScene {gameScene},
    mTutorial {tutorial},
    mSettingsService {settingsService},
    mControlType {settingsService.GetControlType()},
    mScoreManager {field, *this, mediumTextAnimation, effectManager},
    mFieldGravity {field},
    mFieldExplosions {
        engine,
        field,
        mFieldGravity,
        mScoreManager,
        effectManager,
        flyingBlocksSystem
    },
    mFallingPieceAnimation {*this, mFallingPiece},
    mDraggedPiece {gameScene},
    mDraggedPieceAnimation {engine, gameScene, *this, mDraggedPiece},
    mAi {field},
    mDragInputHandler {engine, *this, gameScene, mDraggedPiece},
    mGestureInputHandler {*this, mFallingPiece},
    mClickInputHandler {engine, field, gameScene, *this, tutorial, mAi} {
    
    scrollController.SetGameLogic(*this);
}

void GameLogic::Init(const Level& level) {
    mLevel = &level;
    mControlType = mTutorial.IsGestureControlsAllowed() ? mSettingsService.GetControlType() : ControlType::Click;
    mIsSwipeGhostPieceEnabled = mSettingsService.IsGhostPieceEnabled();

    mFieldGravity.Init();
    mFieldExplosions.Init();
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
    EndCurrentMove();
    mNewMoveReason = NewMoveReason::NewMove;
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mValidMoveBelowDraggedPiece = Pht::Optional<Move> {};
    
    mAllValidMoves = nullptr;
    
    mMovesLeft = mLevel->GetNumMoves();
    mMovesUsed = 0;
    
    mCurrentMove = MoveData {};
    auto& nextPieceGenerator = mCurrentMove.mNextPieceGenerator;
    nextPieceGenerator.Init(mLevel->GetPieceTypes(), mLevel->GetPieceSequence());
    if (!IsFallingPieceVisibleAtNewMove()) {
        mCurrentMove.mPieceType = &nextPieceGenerator.GetNext();
    }
    
    mCurrentMove.mSelectablePieces[0] = &nextPieceGenerator.GetNext();
    mCurrentMove.mSelectablePieces[1] = &nextPieceGenerator.GetNext();
        
    mCurrentMoveTmp = mCurrentMove;
    mPreviousMove = mCurrentMove;
    mShouldUndoMove = false;
    mNumUndosUsed = 0;
    
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
    
    mFallingPiece.ResetBetweenGames();
    
    UpdateLevelProgress();
}

GameLogic::Result GameLogic::Update(bool shouldUpdateLogic, bool shouldUndoMove) {
    UpdateInAnyState(shouldUndoMove);

    switch (mState) {
        case State::LogicUpdate: {
            auto result = UpdateInLogicUpdateState(shouldUpdateLogic);
            if (result != Result::None) {
                return result;
            }
            break;
        }
        case State::FieldExplosions:
            UpdateInFieldExplosionsState();
            break;
    }
    
    return HandleInput();
}

void GameLogic::UpdateInAnyState(bool shouldUndoMove) {
    if (shouldUndoMove) {
        mShouldUndoMove = true;
    }

    if (mLevel->GetObjective() == Level::Objective::BringDownTheAsteroid &&
        mScrollController.IsScrollingDownInClearMode()) {
        
        RemoveBlocksInsideTheShield();
    }
}

GameLogic::Result GameLogic::UpdateInLogicUpdateState(bool shouldUpdateLogic) {
    if (!shouldUpdateLogic) {
        return Result::None;
    }
    
    if (HandleCascading() != CascadeState::NotCascading) {
        return Result::None;
    }
    
    auto settingsChangeResult = HandleSettingsChange();
    if (settingsChangeResult != Result::None) {
        return settingsChangeResult;
    }

    auto newMoveResult = HandleNewMove();
    if (newMoveResult != Result::None) {
        return newMoveResult;
    }
    
    if (HandleUndo()) {
        return Result::None;
    }
    
    UpdateFallingPieceYpos();
    return Result::None;
}

void GameLogic::UpdateInFieldExplosionsState() {
    if (mFieldExplosions.Update() == FieldExplosionsSystem::State::Inactive) {
        mState = State::LogicUpdate;
        RemoveClearedRowsAndPullDownLoosePieces();
    }
    
    UpdateLevelProgress();
}

GameLogic::Result GameLogic::HandleNewMove() {
    if (mNewMoveReason == NewMoveReason::None) {
        return Result::None;
    }
    
    auto result = NewMove(mNewMoveReason);
    mNewMoveReason = NewMoveReason::None;
    return result;
}

GameLogic::Result GameLogic::NewMove(NewMoveReason newMoveReason) {
    assert(newMoveReason != NewMoveReason::None);
    
    if (mBlastArea.IsActive()) {
        mBlastArea.Stop();
    }
    
    UpdateLevelProgress();
    NotifyListenersOfNewMove(newMoveReason);
    
    if (mNumObjectsLeftToClear == 0) {
        return Result::LevelCompleted;
    }
    
    mScene.GetHud().UnHideAllSelectablePreviewPieces();
    ManagePreviewPieces(newMoveReason);
    ManageMoveHistory(newMoveReason);
    mIsOngoingMove = true;
    
    if (mControlType == ControlType::Drag && mMovesLeft == 0) {
        return Result::OutOfMoves;
    }
    
    if (IsFallingPieceVisibleAtNewMove()) {
        return SpawnFallingPiece(FallingPieceSpawnReason::Other, nullptr);
    }
    
    return Result::None;
}

GameLogic::Result GameLogic::SpawnFallingPiece(FallingPieceSpawnReason fallingPieceSpawnReason,
                                               const Piece* pieceType) {
    if (mBlastArea.IsActive()) {
        mBlastArea.Stop();
    }

    ShowFallingPiece();
    auto& spawnedPieceType = pieceType ? *pieceType : *mCurrentMove.mPieceType ;
    auto rotation = CalculateFallingPieceRotation(spawnedPieceType, fallingPieceSpawnReason);
    auto spawnPosition = CalculateFallingPieceSpawnPos(spawnedPieceType, fallingPieceSpawnReason);
    mFallingPiece.Spawn(spawnedPieceType, spawnPosition, rotation, mLevel->GetSpeed());
    
    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(mFallingPiece),
                                                mFallingPiece.GetIntPosition());
    if (mGhostPieceRow > mFallingPiece.GetPosition().y) {
        EndCurrentMove();
        return Result::GameOver;
    }
    
    if (mControlType == ControlType::Swipe && mCurrentMove.mPieceType->IsBomb() &&
        mIsSwipeGhostPieceEnabled &&
        fallingPieceSpawnReason != FallingPieceSpawnReason::BeginDraggingPiece) {

        StartBlastAreaAtGhostPiece();
    }
    
    if (mControlType == ControlType::Click) {
        mClickInputHandler.CalculateMoves(mFallingPiece, GetMovesUsedIncludingCurrent() - 1);
        mClickInputHandler.CreateNewSetOfVisibleMoves();
    }
    
    if (fallingPieceSpawnReason == FallingPieceSpawnReason::Switch) {
        mTutorial.OnSwitchPiece(GetMovesUsedIncludingCurrent(), mFallingPiece.GetPieceType());
    }

    mFallingPieceScaleAnimation.StartScaleUp();
    
    if (mMovesLeft == 0) {
        return Result::OutOfMoves;
    }
    
    return Result::None;
}

void GameLogic::NotifyListenersOfNewMove(NewMoveReason newMoveReason) {
    mScoreManager.OnNewMove();
    
    switch (newMoveReason) {
        case NewMoveReason::NewMove:
            mMediumText.OnNewMove();
            break;
        case NewMoveReason::UndoMove:
            mMediumText.OnNewMoveAfterUndo();
            break;
        case NewMoveReason::None:
            break;
    }
}

void GameLogic::ManagePreviewPieces(NewMoveReason newMoveReason) {
    if (newMoveReason == NewMoveReason::UndoMove ||
        (AreNoMovesUsedYetIncludingUndos() && !IsFallingPieceVisibleAtNewMove())) {

        return;
    }
    
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
    
    if (mShouldUndoMove) {
        mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
    }
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
    if (mIsOngoingMove || mNewMoveReason == NewMoveReason::UndoMove ||
        mDraggedPieceIndex != PreviewPieceIndex::None || AreNoMovesUsedYetIncludingUndos()) {

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

ControlType GameLogic::GetControlType() const {
    return mControlType;
}

void GameLogic::ManageMoveHistory(NewMoveReason newMoveReason) {
    mFallingPiece.UpdateId();

    switch (newMoveReason) {
        case NewMoveReason::NewMove:
            mCurrentMove.mId = mFallingPiece.GetId();
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
        case NewMoveReason::UndoMove:
            mCurrentMove.mId = mFallingPiece.GetId();
            mCurrentMoveTmp = mCurrentMove;
            mPreviousMove = mCurrentMove;
            mScoreManager.OnUndoMove();
            mTutorial.OnNewMove(GetMovesUsedIncludingCurrent());
            break;
        case NewMoveReason::None:
            break;
    }
}

bool GameLogic::AreNoMovesUsedYetIncludingUndos() const {
    return mFallingPiece.GetId() == 0;
}

void GameLogic::EndCurrentMove() {
    mIsOngoingMove = false;
    RemoveFallingPiece();
}

void GameLogic::RemoveFallingPiece() {
    mIsFallingPieceVisible = false;
}

void GameLogic::ShowFallingPiece() {
    mIsFallingPieceVisible = true;
}

void GameLogic::RemoveDraggedPiece() {
    mIsDraggedPieceVisible = false;
}

void GameLogic::ShowDraggedPiece() {
    mIsDraggedPieceVisible = true;
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
            mFallingPiece.GetPieceType().GetDimensions(rotation).mYmin;
        
        auto yAdjust =
            static_cast<float>(previousPieceNumEmptyBottomRows - pieceNumEmptyBottompRows);
        
        return Pht::Vec2 {startXPos + halfColumn, mFallingPiece.GetPosition().y + yAdjust};
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
            return mDraggedPiece.GetRotation();
        case FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece:
            return mCurrentMove.mPreviewPieceRotations.mRotations.mActive;
        default:
            return pieceType.GetSpawnRotation();
    }
}

GameLogic::CascadeState GameLogic::HandleCascading() {
    switch (mCascadeState) {
        case CascadeState::Cascading:
            if (mField.AnyFilledRows()) {
                mCascadeWaitTime = 0.0f;
                mCascadeState = CascadeState::WaitingToClearRows;
            } else {
                mCascadeState = CascadeState::NotCascading;
                mField.ManageBonds();
            }
            break;
        case CascadeState::WaitingToClearRows:
            mCascadeWaitTime += mEngine.GetLastFrameSeconds();
            if ((mCascadeWaitTime > cascadeWaitTime || mCollapsingFieldAnimation.IsInactive())
                && !mScrollController.IsScrolling()) {

                auto removedSubCells = mField.ClearFilledRows();
                HandleClearedFilledRows(removedSubCells);
                UpdateLevelProgress();
                mCollapsingFieldAnimation.GoToInactiveState();
                mCollapsingFieldAnimation.ResetBlockAnimations();
                RemoveClearedRowsAndPullDownLoosePieces();
            }
            break;
        case CascadeState::NotCascading:
            break;
    }
    
    return mCascadeState;
}

void GameLogic::HandleClearedFilledRows(const Field::RemovedSubCells& removedSubCells,
                                        Pht::Optional<int> landedPieceId) {
    PlayClearBlocksSound(mEngine);
    mScoreManager.OnClearedFilledRows(removedSubCells, landedPieceId);
    mFlyingBlocksSystem.AddBlockRows(removedSubCells);
}

GameLogic::Result GameLogic::HandleSettingsChange() {
    auto settingsChanged = false;
    
    if (mSettingsService.GetControlType() != mControlType && mTutorial.IsGestureControlsAllowed()) {
        mCurrentMove.mPreviewPieceRotations = PieceRotations {};
        mCurrentMoveTmp.mPreviewPieceRotations = PieceRotations {};
        mPreviousMove.mPreviewPieceRotations = PieceRotations {};
        
        mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::ActivePieceAfterControlTypeChange;
        
        auto oldControlType = mControlType;
        mControlType = mSettingsService.GetControlType();
        auto result = HandleFallingPieceDuringControlTypeChange(oldControlType, mControlType);
        if (result != Result::None) {
            return result;
        }

        if (mControlType == ControlType::Click) {
            mClickInputHandler.CalculateMoves(mFallingPiece, GetMovesUsedIncludingCurrent() - 1);
            mClickInputHandler.CreateNewSetOfVisibleMoves();
        }
        
        settingsChanged = true;
    }
    
    if (mSettingsService.IsGhostPieceEnabled() != mIsSwipeGhostPieceEnabled) {
        mIsSwipeGhostPieceEnabled = mSettingsService.IsGhostPieceEnabled();
        settingsChanged = true;
    }
    
    if (settingsChanged) {
        ManageBlastArea();
    }
    
    return Result::None;
}

GameLogic::Result GameLogic::HandleFallingPieceDuringControlTypeChange(ControlType oldControlType,
                                                                       ControlType newControlType) {
    if (mLevel->GetSpeed() == 0.0f) {
        switch (oldControlType) {
            case ControlType::Drag:
                return SpawnFallingPiece(FallingPieceSpawnReason::Other, nullptr);
            case ControlType::Click:
            case ControlType::Swipe:
                if (newControlType == ControlType::Drag) {
                    RemoveFallingPiece();
                }
                break;
        }
    }

    return Result::None;
}

void GameLogic::ManageBlastArea() {
    if (mIsSwipeGhostPieceEnabled && mControlType == ControlType::Swipe) {
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

void GameLogic::PrepareForNewMove() {
    mCurrentMoveTmp.mPieceType = mCurrentMove.mPieceType;
    mCurrentMoveTmp.mSelectablePieces = mCurrentMove.mSelectablePieces;
    
    EndCurrentMove();
    mNewMoveReason = NewMoveReason::NewMove;
    
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

bool GameLogic::HandleUndo() {
    if (mShouldUndoMove) {
        mMediumText.StartUndoingMessage();
        UndoMove();
        return true;
    }

    return false;
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
    mNewMoveReason = NewMoveReason::UndoMove;
    mDraggedPieceIndex = PreviewPieceIndex::None;
    EndCurrentMove();
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

void GameLogic::IncreaseScore(int points,
                              const Pht::Vec2& scoreTextPosition,
                              float scoreTextDelay) {
    mCurrentMove.mScore += points;
    mScoreTexts.Start(points, scoreTextPosition, scoreTextDelay);
}

int GameLogic::CalculateBonusPointsAtLevelCompleted() const {
    return mScoreManager.CalculateBonusPointsAtLevelCompleted(mMovesLeft);
}

const FallingPiece* GameLogic::GetFallingPiece() const {
    if (mIsFallingPieceVisible) {
        return &mFallingPiece;
    }
    
    return nullptr;
}

const DraggedPiece* GameLogic::GetDraggedPiece() const {
    if (mIsDraggedPieceVisible) {
        return &mDraggedPiece;
    }
    
    return nullptr;
}

bool GameLogic::IsFallingPieceVisibleAtNewMove() const {
    if (mControlType == ControlType::Drag && mLevel->GetSpeed() == 0.0f) {
        return false;
    }
    
    return true;
}

void GameLogic::UpdateFallingPieceYpos() {
    if (!IsFallingPieceVisibleAtNewMove()) {
        return;
    }

    mFallingPiece.UpdateTime(mEngine.GetLastFrameSeconds());

    auto newYPosition =
        mFallingPiece.GetPosition().y - mFallingPiece.GetSpeed() * mEngine.GetLastFrameSeconds();

    switch (mFallingPiece.GetState()) {
        case FallingPiece::State::Falling:
        case FallingPiece::State::BeingDraggedDown:
            if (newYPosition < mGhostPieceRow) {
                mFallingPiece.GoToLandingState(mGhostPieceRow);
            } else {
                mFallingPiece.SetY(newYPosition);
                if (mFallingPiece.GetPreviousIntY() != static_cast<int>(newYPosition)) {
                    if (mControlType == ControlType::Click) {
                        mClickInputHandler.UpdateMoves(mFallingPiece,
                                                       GetMovesUsedIncludingCurrent() - 1);
                    } else {
                        auto& validMoves = mAi.FindValidMoves(mFallingPiece,
                                                              GetMovesUsedIncludingCurrent() - 1);
                        mAllValidMoves = &validMoves.mMoves;
                        if (mPiecePathSystem.IsPathVisible() && mValidMoveBelowDraggedPiece.HasValue()) {
                            if (auto* move = mAllValidMoves->Find(mValidMoveBelowDraggedPiece.GetValue())) {
                                auto lowestVisibleRow =
                                    static_cast<int>(mScrollController.GetLowestVisibleRow());
                                mPiecePathSystem.ShowPath(mFallingPiece,
                                                          *(move->mLastMovement),
                                                          lowestVisibleRow);
                            }
                        }
                    }
                }
            }
            break;
        case FallingPiece::State::Landing:
            if (mFallingPiece.GetPosition().y != mGhostPieceRow) {
                mFallingPiece.GoToFallingState(mLevel->GetSpeed());
            } else {
                auto lastFrameSeconds {mEngine.GetLastFrameSeconds()};
                mFallingPiece.IncreaseTimeSinceTouchdown(lastFrameSeconds);
                mFallingPiece.IncreaseTimeSinceMovement(lastFrameSeconds);
                if (mFallingPiece.GetTimeSinceTouchdown() > mLandingMovementDuration ||
                    mFallingPiece.GetTimeSinceMovement() > mLandingNoMovementDuration) {
                    if (mIsDraggedPieceVisible) {
                        CancelDraggingBecausePieceLands();
                    }
                    LandFallingPiece(false);
                }
            }
            break;
    }
}

void GameLogic::DropFallingPiece() {
    bool finalMovementWasADrop {mFallingPiece.GetPosition().y > mGhostPieceRow + 1};
    mFallingPiece.SetY(mGhostPieceRow);
    
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
    
    auto clearedAnyFilledRows = false;
    auto& pieceType = mFallingPiece.GetPieceType();
    if (IsBomb(pieceType)) {
        if (finalMovementWasADrop) {
            mPieceDropParticleEffect.StartEffect(mFallingPiece);
        }

        DetonateDroppedBomb();
    } else {
        auto impactedLevelBombs =
            mField.DetectImpactedBombs(CreatePieceBlocks(mFallingPiece),
                                       mFallingPiece.GetIntPosition());
        auto startBounceAnimation = false;
        
        if (finalMovementWasADrop) {
            startBounceAnimation = true;
            mCollapsingFieldAnimation.GoToBlocksBouncingState();
            mPieceTrailParticleEffect.StartEffect(mFallingPiece);
        }
        
        mFlashingBlocksAnimation.ResetFlashingBlockAnimations();
        mField.LandFallingPiece(mFallingPiece, startBounceAnimation);
        mFlashingBlocksAnimation.Start(pieceType.GetColor());
        DetonateImpactedLevelBombs(impactedLevelBombs);
        
        switch (mLevel->GetObjective()) {
            case Level::Objective::Clear:
            case Level::Objective::BringDownTheAsteroid: {
                auto removedSubCells = mField.ClearFilledRows();
                if (!removedSubCells.IsEmpty()) {
                    clearedAnyFilledRows = true;
                    HandleClearedFilledRows(removedSubCells, mFallingPiece.GetId());
                    mCollapsingFieldAnimation.GoToInactiveState();
                    mCollapsingFieldAnimation.ResetBlockAnimations();
                    if (impactedLevelBombs.IsEmpty()) {
                        RemoveClearedRowsAndPullDownLoosePieces();
                    }
                }
                break;
            }
            case Level::Objective::Build: {
                auto slotsCoveredByPiece = mField.CalculatePieceFilledSlots(mFallingPiece);
                mScoreManager.OnFilledSlots(slotsCoveredByPiece);
                break;
            }
        }
    }
    
    if (!clearedAnyFilledRows) {
        mScoreManager.OnClearedNoFilledRows();
    }
    
    if (mLevel->GetObjective() == Level::Objective::BringDownTheAsteroid) {
        RemoveBlocksInsideTheShield();
    }
    
    if (mState != State::FieldExplosions && mCascadeState == CascadeState::NotCascading) {
        mField.ManageBonds();
        PlayLandPieceSound();
    }
    
    PrepareForNewMove();
}

void GameLogic::DetonateDroppedBomb() {
    GoToFieldExplosionsState();
    
    auto impactedLevelBombs =
        mField.DetectImpactedBombs(CreatePieceBlocks(mFallingPiece),
                                   mFallingPiece.GetIntPosition());

    auto intPieceDetonationPos = mFallingPiece.GetIntPosition() + Pht::IVec2{1, 1};
    auto pieceDetonationPos = mFallingPiece.GetRenderablePosition() + Pht::Vec2{1.0f, 1.0f};
    
    if (mFallingPiece.GetPieceType().IsRowBomb()) {
        if (!impactedLevelBombs.IsEmpty()) {
            auto& impactedLevelBomb = impactedLevelBombs.Front();
            switch (impactedLevelBomb.mKind) {
                case BlockKind::Bomb:
                    mFieldExplosions.DetonateLevelBomb(impactedLevelBomb.mPosition);
                    break;
                case BlockKind::RowBomb:
                    mFieldExplosions.DetonateRowBomb(impactedLevelBomb.mPosition);
                    break;
                default:
                    break;
            }
        }
        
        mFieldExplosions.DetonateRowBomb(intPieceDetonationPos, pieceDetonationPos);
    } else {
        if (!impactedLevelBombs.IsEmpty() && impactedLevelBombs.Front().mKind == BlockKind::Bomb) {
            mFieldExplosions.DetonateBigBomb(intPieceDetonationPos);
        } else {
            mFieldExplosions.DetonateBomb(intPieceDetonationPos, pieceDetonationPos);
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
                mFieldExplosions.DetonateLevelBomb(impactedLevelBomb.mPosition);
                break;
            case BlockKind::RowBomb:
                mFieldExplosions.DetonateRowBomb(impactedLevelBomb.mPosition);
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
        case ControlType::Drag:
        case ControlType::Click:
            audio.PlaySound(landingSoundResourceId);
            break;
        case ControlType::Swipe:
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
        mFlyingBlocksSystem.AddBlocksRemovedByTheShield(removedSubCells, mField.GetNumColumns());
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

void GameLogic::RotateFallingPiece(const Pht::TouchEvent& touchEvent) {
    auto& pieceType = mFallingPiece.GetPieceType();
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
    
    auto position = mFallingPiece.GetIntPosition();
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
        if (!CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                             position,
                                                             newRotation,
                                                             pieceType)) {
            mFallingPiece.SetRotation(newRotation);
        }
    }

    mGhostPieceRow = mField.DetectCollisionDown(CreatePieceBlocks(mFallingPiece),
                                                mFallingPiece.GetIntPosition());
}

Rotation GameLogic::CalculateNewRotation(const Pht::TouchEvent& touchEvent) {
    auto numRotations = mFallingPiece.GetPieceType().GetNumRotations();
    auto rotationInt = static_cast<int>(mFallingPiece.GetRotation());
    
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
                !CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                                 Pht::IVec2{freeXPos, position.y},
                                                                 newRotation,
                                                                 mFallingPiece.GetPieceType())) {
                mFallingPiece.SetX(freeXPos + halfColumn);
                mFallingPiece.SetRotation(newRotation);
            }
            break;
        }
        case Direction::Left: {
            auto freeXPos = mField.DetectFreeSpaceRight(pieceBlocks, position);
            if (freeXPos - position.x <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                                 Pht::IVec2{freeXPos, position.y},
                                                                 newRotation,
                                                                 mFallingPiece.GetPieceType())) {
                mFallingPiece.SetX(freeXPos + halfColumn);
                mFallingPiece.SetRotation(newRotation);
            }
            break;
        }
        case Direction::Up: {
            auto freeYPos = mField.DetectFreeSpaceDown(pieceBlocks, position);
            if (position.y - freeYPos <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                                 Pht::IVec2{position.x, freeYPos},
                                                                 newRotation,
                                                                 mFallingPiece.GetPieceType())) {
                mFallingPiece.SetY(freeYPos);
                mFallingPiece.SetRotation(newRotation);
            }
            break;
        }
        case Direction::Down: {
            auto freeYPos = mField.DetectFreeSpaceUp(pieceBlocks, position);
            if (freeYPos - position.y <= maxRotateAdjustment &&
                !CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                                 Pht::IVec2{position.x, freeYPos},
                                                                 newRotation,
                                                                 mFallingPiece.GetPieceType())) {
                mFallingPiece.SetY(freeYPos);
                mFallingPiece.SetRotation(newRotation);
            }
            break;
        }
        default:
            break;
    }
}

void GameLogic::SwitchPiece() {
    if (!mTutorial.IsSwitchPieceAllowed()) {
        return;
    }
    
    auto& nextPieceType = *mCurrentMove.mSelectablePieces[0];
    if (!IsThereRoomToSwitchPiece(nextPieceType, nextPieceType.GetSpawnRotation())) {
        mMediumText.StartNoRoomMessage();
        return;
    }
    
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::SwitchPiece));
    
    auto* previousActivePieceType = mCurrentMove.mPieceType;
    
    mCurrentMove.mPieceType = mCurrentMove.mSelectablePieces[0];
    mCurrentMove.mSelectablePieces[0] = mCurrentMove.mSelectablePieces[1];
    mCurrentMove.mSelectablePieces[1] = previousActivePieceType;
    mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::SwitchPiece;
    mCurrentMove.mPreviewPieceRotations = PieceRotations {};
    
    SpawnFallingPiece(FallingPieceSpawnReason::Switch, mCurrentMove.mPieceType);
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
    auto pieceBlocks = CreatePieceBlocks(mFallingPiece);

    if (fallingPieceNewX - mFallingPiece.GetPosition().x > 0.0f) {
        auto collisionRight =
            mField.DetectCollisionRight(pieceBlocks, mFallingPiece.GetIntPosition()) +
            halfColumn;
        
        mFallingPiece.SetX(std::fmin(collisionRight, fallingPieceNewX));
    } else {
        auto collisionLeft =
            mField.DetectCollisionLeft(pieceBlocks, mFallingPiece.GetIntPosition()) +
            halfColumn;
        
        mFallingPiece.SetX(std::fmax(collisionLeft, fallingPieceNewX));
    }

    mGhostPieceRow = mField.DetectCollisionDown(pieceBlocks, mFallingPiece.GetIntPosition());
    
    if (mBlastArea.IsActive()) {
        SetBlastAreaPositionAtGhostPiece();
        Pht::IVec2 ghostPiecePosition {mFallingPiece.GetIntPosition().x, mGhostPieceRow};
        auto blastRadiusKind = CalculateBlastRadiusKind(CreatePieceBlocks(mFallingPiece),
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
    if (!mIsFallingPieceVisible) {
        return;
    }
    
    Pht::IVec2 ghostPiecePosition {mFallingPiece.GetIntPosition().x, mGhostPieceRow};
    mBlastArea.Start(CalculateBlastRadiusKind(CreatePieceBlocks(mFallingPiece),
                                              ghostPiecePosition));
    SetBlastAreaPositionAtGhostPiece();
}

void GameLogic::SetBlastAreaPositionAtGhostPiece() {
    if (!mIsFallingPieceVisible) {
        return;
    }

    Pht::Vec2 blastRadiusAnimationPos {
        mFallingPiece.GetRenderablePosition().x + 1.0f,
        static_cast<float>(mGhostPieceRow) + 1.0f
    };

    mBlastArea.SetPosition(blastRadiusAnimationPos);
}

void GameLogic::StartBlastArea(const Pht::IVec2& position) {
    mBlastArea.Start(CalculateBlastRadiusKind(CreatePieceBlocks(mFallingPiece), position));
    
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

bool GameLogic::HandleBeginDraggingPiece(PreviewPieceIndex draggedPieceIndex) {
    if (draggedPieceIndex == PreviewPieceIndex::None) {
        return false;
    }
    
    ShowDraggedPiece();
    auto& pieceType = mDraggedPiece.GetPieceType();
    if (!IsThereRoomToSwitchPiece(pieceType, mDraggedPiece.GetRotation())) {
        mMediumText.StartNoRoomMessage();
        RemoveDraggedPiece();
        return false;
    }
    
    mDraggedPieceIndex = draggedPieceIndex;
    if (SpawnFallingPiece(FallingPieceSpawnReason::BeginDraggingPiece, &pieceType) != Result::None) {
        RemoveDraggedPiece();
        mDraggedPieceIndex = PreviewPieceIndex::None;
        return false;
    }
    
    mScene.GetHud().HidePreviewPiece(draggedPieceIndex);
    
    auto& validMoves = mAi.FindValidMoves(mFallingPiece, GetMovesUsedIncludingCurrent() - 1);
    mAllValidMoves = &validMoves.mMoves;
    
    mDraggedPieceAnimation.StartGoUpAnimation();
    UpdateDraggedGhostPieceRowAndBlastArea();
    return true;
}

void GameLogic::HandleDraggedPieceMoved() {
    auto pieceBlocks = CreatePieceBlocks(mFallingPiece);
    auto fallingPieceNewX =
        static_cast<float>(mDraggedPiece.GetFieldGridPosition().x) + halfColumn;

    if (fallingPieceNewX - mFallingPiece.GetPosition().x > 0.0f) {
        auto collisionRight =
            mField.DetectCollisionRight(pieceBlocks, mFallingPiece.GetIntPosition()) + halfColumn;
        
        mFallingPiece.SetX(std::fmin(collisionRight, fallingPieceNewX));
    } else {
        auto collisionLeft =
            mField.DetectCollisionLeft(pieceBlocks, mFallingPiece.GetIntPosition()) + halfColumn;
        
        mFallingPiece.SetX(std::fmax(collisionLeft, fallingPieceNewX));
    }

    UpdateDraggedGhostPieceRowAndBlastArea();
}

void GameLogic::HandleDragPieceTouchEnd() {
    auto ghostPieceRow = 0;
    if (auto* move = GetValidMoveBelowDraggedPiece(ghostPieceRow)) {
        SelectMove(*move);
        RemoveDraggedPiece();
    } else {
        mDraggedPieceAnimation.StartGoBackAnimation(mDraggedPieceIndex);
        mFallingPieceScaleAnimation.StartScaleDown();
    }
    
    HidePiecePath();
}

void GameLogic::EndPieceDrag() {
    if (IsFallingPieceVisibleAtNewMove()) {
        SpawnFallingPiece(FallingPieceSpawnReason::RespawnActiveAfterStopDraggingPiece,
                          mCurrentMove.mPieceType);
    } else {
        RemoveFallingPiece();
    }

    mScene.GetHud().ShowPreviewPiece(mDraggedPieceIndex);
    HidePiecePath();
    mDraggedPieceIndex = PreviewPieceIndex::None;
    RemoveDraggedPiece();
}

void GameLogic::CancelDraggingBecausePieceLands() {
    HidePiecePath();
    RemoveDraggedPiece();
    mDragInputHandler.EndDrag();
}

void GameLogic::OnDraggedPieceGoingBackAnimationFinished() {
    EndPieceDrag();
}

void GameLogic::UpdateDraggedGhostPieceRowAndBlastArea() {
    auto ghostPieceRow = 0;
    if (auto* move = GetValidMoveBelowDraggedPiece(ghostPieceRow)) {
        mDraggedGhostPieceRow = ghostPieceRow;
        
        auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
        mPiecePathSystem.ShowPath(mFallingPiece, *(move->mLastMovement), lowestVisibleRow);
        mValidMoveBelowDraggedPiece = *move;
        
        if (mDraggedPiece.GetPieceType().IsBomb()) {
            Pht::IVec2 ghostPiecePosition {
                mDraggedPiece.GetFieldGridPosition().x,
                mDraggedGhostPieceRow.GetValue()
            };
        
            Pht::Vec2 blastRadiusAnimationPos {
                static_cast<float>(ghostPiecePosition.x) + 1.0f,
                static_cast<float>(ghostPiecePosition.y) + 1.0f
            };
            
            mBlastArea.SetPosition(blastRadiusAnimationPos);
            auto blastRadiusKind = CalculateBlastRadiusKind(CreatePieceBlocks(mDraggedPiece),
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
        HidePiecePath();

        if (mBlastArea.IsActive()) {
            mBlastArea.Stop();
        }
    }
}

const Move* GameLogic::GetValidMoveBelowDraggedPiece(int& ghostPieceRow) {
    auto pieceBlocks = CreatePieceBlocks(mDraggedPiece);
    auto piecePosition = mDraggedPiece.GetFieldGridPosition();
    Field::CollisionResult collisionResult;
    mField.CheckCollision(collisionResult, pieceBlocks, piecePosition, Pht::IVec2{0, 0}, false);
    if (collisionResult.mIsCollision != IsCollision::Yes) {
        auto rotation = mDraggedPiece.GetRotation();
        auto rowIfDropped = mField.DetectCollisionDown(pieceBlocks, piecePosition);
        Pht::IVec2 positionIfDropped {piecePosition.x, rowIfDropped};
        Move moveIfDropped {.mPosition = positionIfDropped, rotation};
        if (auto* validMove = mAllValidMoves->Find(moveIfDropped)) {
            ghostPieceRow = positionIfDropped.y;
            return validMove;
        }

        auto& duplicateMoveCheck = mDraggedPiece.GetPieceType().GetDuplicateMoveCheck(rotation);
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

void GameLogic::HidePiecePath() {
    mPiecePathSystem.HidePath();
    mValidMoveBelowDraggedPiece = Pht::Optional<Move> {};
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
        case ControlType::Drag:
            mDragInputHandler.HandleTouch(touchEvent);
            break;
        case ControlType::Click:
            mClickInputHandler.HandleTouch(touchEvent, GetMovesUsedIncludingCurrent());
            break;
        case ControlType::Swipe:
            if (mDragInputHandler.HandleTouch(touchEvent) == DragInputHandler::State::Idle) {
                mGestureInputHandler.HandleTouch(touchEvent);
            }
            break;
    }
}

bool GameLogic::IsInputAllowed() const {
    return mIsOngoingMove &&
           mFallingPieceAnimation.GetState() == FallingPieceAnimation::State::Inactive &&
           (mDraggedPieceAnimation.GetState() == DraggedPieceAnimation::State::Inactive ||
            mDraggedPieceAnimation.GetState() == DraggedPieceAnimation::State::DraggedPieceGoingUp);
}
