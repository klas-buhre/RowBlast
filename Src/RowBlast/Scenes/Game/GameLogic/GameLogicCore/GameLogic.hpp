#ifndef GameLogic_hpp
#define GameLogic_hpp

// Engine includes.
#include "Optional.hpp"

// Game includes.
#include "IGameLogic.hpp"
#include "FallingPiece.hpp"
#include "DraggedPiece.hpp"
#include "NextPieceGenerator.hpp"
#include "CollisionDetection.hpp"
#include "Piece.hpp"
#include "Field.hpp"
#include "Ai.hpp"
#include "DragInputHandler.hpp"
#include "GestureInputHandler.hpp"
#include "ClickInputHandler.hpp"
#include "FallingPieceAnimation.hpp"
#include "DraggedPieceAnimation.hpp"
#include "FieldGravitySystem.hpp"
#include "FieldExplosionsSystem.hpp"
#include "BlastArea.hpp"
#include "ScoreManager.hpp"
#include "SettingsService.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Level;
    class ScrollController;
    class EffectManager;
    class FlyingBlocksSystem;
    class CollapsingFieldAnimationSystem;
    class PieceDropParticleEffect;
    class PieceTrailParticleEffect;
    class PiecePathSystem;
    class GameHudController;
    class GameScene;
    class Tutorial;
    class UserServices;
    class FlashingBlocksAnimationSystem;
    class FallingPieceScaleAnimation;
    class Shield;
    class ScoreTexts;
    class MediumText;
    
    enum class PreviewPieceAnimationToStart {
        NextPieceAndSwitch,
        NextPieceAndRefillActive,
        NextPieceAndRefillSelectable0,
        NextPieceAndRefillSelectable1,
        SwitchPiece,
        RemoveActivePiece,
        ActivePieceAfterControlTypeChange,
        None
    };
    
    struct PreviewPieceRotations {
        Rotation mActive {Rotation::Deg0};
        Rotation mSelectable0 {Rotation::Deg0};
        Rotation mSelectable1 {Rotation::Deg0};
        
        bool operator==(const PreviewPieceRotations& other) const {
            return mActive == other.mActive && mSelectable0 == other.mSelectable0 &&
                   mSelectable1 == other.mSelectable1;
        }
        
        bool operator!=(const PreviewPieceRotations& other) const {
            return !(*this == other);
        }
    };

    class GameLogic: public IGameLogic {
    public:
        enum class Result {
            None,
            Pause,
            LevelCompleted,
            OutOfMoves,
            GameOver
        };
        
        GameLogic(Pht::IEngine& engine,
                  Field& field,
                  ScrollController& scrollController,
                  GameScene& gameScene,
                  EffectManager& effectManger,
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
                  const SettingsService& settingsService);
        
        void DropFallingPiece() override;
        void SelectMove(const Move& move) override;
        void OnFallingPieceAnimationFinished(bool finalMovementWasADrop) override;
        void RotatePreviewPiece(PreviewPieceIndex previewPieceIndex) override;
        void RotatePreviewPieces() override;
        void RotateFallingPiece(const Pht::TouchEvent& touchEvent) override;
        void SwitchPiece() override;
        void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) override;
        int GetGhostPieceRow() const override;
        bool IsInFieldExplosionsState() const override;
        void StartBlastArea(const Pht::IVec2& position) override;
        void StopBlastArea() override;
        bool HandleBeginDraggingPiece(PreviewPieceIndex draggedPieceIndex) override;
        void HandleDraggedPieceMoved() override;
        void HandleDragPieceTouchEnd() override;
        void OnDraggedPieceGoingBackAnimationFinished() override;
        void EndPieceDrag() override;
        bool IsDragPieceAllowed(PreviewPieceIndex draggedPieceIndex,
                                Rotation rotation) const override;
        void IncreaseScore(int points, const Pht::Vec2& scoreTextPosition) override;
        void IncreaseScore(int points,
                           const Pht::Vec2& scoreTextPosition,
                           float scoreTextDelay) override;
        const Piece* GetPieceType() const override;
        const TwoPieces& GetSelectablePieces() const override;
        const PreviewPieceRotations& GetPreviewPieceRotations() const override;
        const PreviewPieceRotations& GetPreviewPieceHudRotations() const override;
        ControlType GetControlType() const override;
        
        void Init(const Level& level);
        Result Update(bool shouldUpdateLogic, bool shouldUndoMove);
        bool IsUndoMovePossible() const;
        bool HandleUndo();
        void UndoMove();
        int GetMovesUsedIncludingCurrent() const;
        int CalculateBonusPointsAtLevelCompleted() const;
        const FallingPiece* GetFallingPiece() const;
        const DraggedPiece* GetDraggedPiece() const;
        
        void SetMovesLeft(int movesLeft) {
            mMovesLeft = movesLeft;
        }
        
        FallingPieceAnimation& GetFallingPieceAnimation() {
            return mFallingPieceAnimation;
        }
        
        DraggedPieceAnimation& GetDraggedPieceAnimation() {
            return mDraggedPieceAnimation;
        }
        
        Pht::Optional<int> GetDraggedGhostPieceRow() const {
            return mDraggedGhostPieceRow;
        }
        
        bool IsSwipeGhostPieceEnabled() const {
            return mIsSwipeGhostPieceEnabled;
        }

        const NextPieceGenerator& GetNextPieceGenerator() const {
            return mCurrentMove.mNextPieceGenerator;
        }
        
        PreviewPieceAnimationToStart GetPreviewPieceAnimationToStart() const {
            return mPreviewPieceAnimationToStart;
        }

        void ResetPreviewPieceAnimationToStart() {
            mPreviewPieceAnimationToStart = PreviewPieceAnimationToStart::None;
        }

        int GetMovesLeft() const {
            return mMovesLeft;
        }
        
        const ClickInputHandler& GetClickInputHandler() const {
            return mClickInputHandler;
        }
        
        bool IsCascading() const {
            return mCascadeState == CascadeState::Cascading;
        }
        
        int GetNumObjectsLeftToClear() const {
            return mNumObjectsLeftToClear;
        }
        
        int GetScore() const {
            return mCurrentMove.mScore;
        }
        
        void SetScore(int score) {
            mCurrentMove.mScore = score;
        }
        
    private:
        enum class NewMoveReason {
            None,
            NewMove,
            UndoMove
        };

        enum class FallingPieceSpawnReason {
            Switch,
            BeginDraggingPiece,
            RespawnActiveAfterStopDraggingPiece,
            Other
        };
        
        enum class State {
            LogicUpdate,
            FieldExplosions
        };
        
        enum class CascadeState {
            NotCascading,
            Cascading,
            WaitingToClearRows
        };

        void UpdateInAnyState(bool shouldUndoMove);
        Result UpdateInLogicUpdateState(bool shouldUpdateLogic);
        void UpdateInFieldExplosionsState();
        Result HandleNewMove();
        Result NewMove(NewMoveReason newMoveReason);
        Result SpawnFallingPiece(FallingPieceSpawnReason fallingPieceSpawnReason,
                                 const Piece* pieceType);
        void NotifyListenersOfNewMove(NewMoveReason newMoveReason);
        void ManagePreviewPieces(NewMoveReason newMoveReason);
        void SetPreviewPiece(PreviewPieceIndex previewPieceIndex, const Piece* pieceType);
        void ManageMoveHistory(NewMoveReason newMoveReason);
        void StartBlastAreaAtGhostPiece();
        void SetBlastAreaPositionAtGhostPiece();
        bool AreNoMovesUsedYetIncludingUndos() const;
        void EndCurrentMove();
        void ShowFallingPiece();
        void RemoveFallingPiece();
        void ShowDraggedPiece();
        void RemoveDraggedPiece();
        void PrepareForNewMove();
        void ManageBlastArea();
        void UpdateLevelProgress();
        Pht::Vec2 CalculateFallingPieceSpawnPos(const Piece& pieceType,
                                                FallingPieceSpawnReason fallingPieceSpawnReason);
        Rotation CalculateFallingPieceRotation(const Piece& pieceType,
                                               FallingPieceSpawnReason fallingPieceSpawnReason);
        CascadeState HandleCascading();
        void HandleClearedFilledRows(const Field::RemovedSubCells& removedSubCells,
                                     Pht::Optional<int> landedPieceId = Pht::Optional<int>{});
        Result HandleSettingsChange();
        Result HandleFallingPieceDuringControlTypeChange(ControlType oldControlType,
                                                         ControlType newControlType);
        bool IsFallingPieceVisibleAtNewMove() const;
        void UpdateFallingPieceYpos();
        void LandFallingPiece(bool finalMovementWasADrop);
        void DetonateDroppedBomb();
        void DetonateImpactedLevelBombs(const Field::ImpactedBombs& impactedLevelBombs);
        void GoToFieldExplosionsState();
        void RemoveClearedRowsAndPullDownLoosePieces(bool doBounceCalculations = true,
                                                     bool resetIsPulledDownFlags = true);
        void PullDownLoosePiecesClearObjective();
        void PullDownLoosePiecesAsteroidObjective();
        void RotatePreviewPieces(PreviewPieceRotations& previewPieceRotations,
                                 Pht::Optional<int> numRotations);
        void RotatePreviewPiece(Rotation& previewPieceRotation,
                                const Piece* pieceType,
                                Pht::Optional<int> numRotations);
        void RotateHudPreviewPiece(Rotation& previewPieceRotation, const Piece* pieceType);
        Rotation CalculateNewRotation(const Pht::TouchEvent& touchEvent);
        void RotatateAndAdjustPosition(Rotation newRotation,
                                       const PieceBlocks& pieceBlocks,
                                       const Pht::IVec2& position,
                                       Direction collisionDirection);
        BlastArea::Kind CalculateBlastRadiusKind(const PieceBlocks& pieceBlocks,
                                                 const Pht::IVec2& position);
        void PlayLandPieceSound();
        void RemoveBlocksInsideTheShield();
        bool IsThereRoomToSwitchPiece(const Piece& pieceType, Rotation rotation);
        void UpdateDraggedGhostPieceRowAndBlastArea();
        const Move* GetValidMoveBelowDraggedPiece(int& ghostPieceRow);
        void HidePiecePath();
        void CancelDraggingBecausePieceLands();
        Result HandleInput();
        void ForwardTouchToInputHandler(const Pht::TouchEvent& touchEvent);
        bool IsInputAllowed() const;
        
        struct PieceRotations {
            PreviewPieceRotations mRotations;
            PreviewPieceRotations mHudRotations;
        };
        
        struct MoveData {
            NextPieceGenerator mNextPieceGenerator;
            const Piece* mPieceType {nullptr};
            TwoPieces mSelectablePieces;
            PieceRotations mPreviewPieceRotations;
            int mId {0};
            int mScore {0};
        };
        
        Pht::IEngine& mEngine;
        Field& mField;
        ScrollController& mScrollController;
        FlyingBlocksSystem& mFlyingBlocksSystem;
        FlashingBlocksAnimationSystem& mFlashingBlocksAnimation;
        CollapsingFieldAnimationSystem& mCollapsingFieldAnimation;
        EffectManager& mEffectManager;
        PieceDropParticleEffect& mPieceDropParticleEffect;
        PieceTrailParticleEffect& mPieceTrailParticleEffect;
        PiecePathSystem& mPiecePathSystem;
        BlastArea& mBlastArea;
        FallingPieceScaleAnimation& mFallingPieceScaleAnimation;
        Shield& mShield;
        ScoreTexts& mScoreTexts;
        MediumText& mMediumText;
        GameHudController& mGameHudController;
        GameScene& mScene;
        Tutorial& mTutorial;
        const SettingsService& mSettingsService;
        ControlType mControlType;
        bool mIsSwipeGhostPieceEnabled {false};
        State mState {State::LogicUpdate};
        CascadeState mCascadeState {CascadeState::NotCascading};
        float mCascadeWaitTime {0.0f};
        ScoreManager mScoreManager;
        FallingPiece mFallingPiece;
        DraggedPiece mDraggedPiece;
        FieldGravitySystem mFieldGravity;
        FieldExplosionsSystem mFieldExplosions;
        FallingPieceAnimation mFallingPieceAnimation;
        DraggedPieceAnimation mDraggedPieceAnimation;
        Ai mAi;
        Moves* mAllValidMoves {nullptr};
        DragInputHandler mDragInputHandler;
        GestureInputHandler mGestureInputHandler;
        ClickInputHandler mClickInputHandler;
        bool mIsFallingPieceVisible {false};
        bool mIsDraggedPieceVisible {false};
        bool mIsOngoingMove {false};
        NewMoveReason mNewMoveReason {NewMoveReason::None};
        MoveData mCurrentMove;
        MoveData mCurrentMoveTmp;
        MoveData mPreviousMove;
        PreviewPieceIndex mDraggedPieceIndex {PreviewPieceIndex::None};
        PreviewPieceAnimationToStart mPreviewPieceAnimationToStart {PreviewPieceAnimationToStart::None};
        const Level* mLevel {nullptr};
        float mLandingNoMovementDuration {0.0f};
        float mLandingMovementDuration {0.0f};
        int mGhostPieceRow {0};
        Pht::Optional<int> mDraggedGhostPieceRow;
        Pht::Optional<Move> mValidMoveBelowDraggedPiece;
        int mMovesUsed {0};
        int mMovesLeft {0};
        int mNumObjectsLeftToClear {0};
        bool mShouldUndoMove {false};
        int mNumUndosUsed {0};
    };
}

#endif
