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
#include "FieldGravity.hpp"
#include "FieldExplosionsStates.hpp"
#include "BlastArea.hpp"
#include "ComboDetector.hpp"
#include "SettingsService.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Level;
    class ScrollController;
    class EffectManager;
    class FlyingBlocksAnimation;
    class CollapsingFieldAnimation;
    class PieceDropParticleEffect;
    class PieceTrailParticleEffect;
    class GameHudController;
    class GameScene;
    class Tutorial;
    class UserServices;
    class FlashingBlocksAnimation;
    class FallingPieceScaleAnimation;
    class Shield;
    class SmallText;
    class ValidAreaAnimation;
    
    enum class PreviewPieceAnimationToStart {
        NextPieceAndSwitch,
        NextPieceAndRefillActive,
        NextPieceAndRefillSelectable0,
        NextPieceAndRefillSelectable1,
        SwitchPiece,
        RemoveActivePiece,
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
                  FlyingBlocksAnimation& flyingBlocksAnimation,
                  FlashingBlocksAnimation& flashingBlocksAnimation,
                  CollapsingFieldAnimation& collapsingFieldAnimation,
                  PieceDropParticleEffect& pieceDropParticleEffect,
                  PieceTrailParticleEffect& pieceTrailParticleEffect,
                  BlastArea& blastRadiusAnimation,
                  FallingPieceScaleAnimation& fallingPieceScaleAnimation,
                  Shield& shieldAnimation,
                  ValidAreaAnimation& validAreaAnimation,
                  SmallText& smallTextAnimation,
                  GameHudController& gameHudController,
                  Tutorial& tutorial,
                  const SettingsService& settingsService);
        
        void DropFallingPiece() override;
        void SelectMove(const Move& move) override;
        void OnFallingPieceAnimationFinished(bool finalMovementWasADrop) override;
        void RotatePreviewPiece(PreviewPieceIndex previewPieceIndex) override;
        void RotatePreviewPieces() override;
        void RotatePiece(const Pht::TouchEvent& touchEvent) override;
        void SwitchPiece() override;
        void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) override;
        int GetGhostPieceRow() const override;
        bool IsInFieldExplosionsState() const override;
        void StartBlastArea(const Pht::IVec2& position) override;
        void StopBlastArea() override;
        bool BeginDraggingPiece(PreviewPieceIndex draggedPieceIndex) override;
        void OnDraggedPieceMoved() override;
        void StopDraggingPiece() override;
        void OnDraggedPieceAnimationFinished() override;
        void CancelDraggingPiece() override;
        const Piece* GetPieceType() const override;
        const TwoPieces& GetSelectablePieces() const override;
        const PreviewPieceRotations& GetPreviewPieceRotations() const override;
        const PreviewPieceRotations& GetPreviewPieceHudRotations() const override;
        
        void Init(const Level& level);
        Result Update(bool shouldUpdateLogic, bool shouldUndoMove);
        bool IsUndoMovePossible() const;
        void UndoMove();
        int GetMovesUsedIncludingCurrent() const;
        
        void SetMovesLeft(int movesLeft) {
            mMovesLeft = movesLeft;
        }
        
        FallingPieceAnimation& GetFallingPieceAnimation() {
            return mFallingPieceAnimation;
        }
        
        DraggedPieceAnimation& GetDraggedPieceAnimation() {
            return mDraggedPieceAnimation;
        }
        
        const FallingPiece* GetFallingPiece() const {
            return mFallingPiece;
        }

        const DraggedPiece* GetDraggedPiece() const {
            return mDraggedPiece;
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
        
        bool IsUsingClickControls() const {
            return mControlType == ControlType::Click;
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
        
    private:
        enum class FallingPieceSpawnReason {
            None,
            NextMove,
            Switch,
            BeginDraggingPiece,
            RespawnActiveAfterStopDraggingPiece,
            UndoMove
        };

        Result SpawnFallingPiece(FallingPieceSpawnReason fallingPieceSpawnReason);
        const Piece& CalculatePieceType(FallingPieceSpawnReason fallingPieceSpawnReason);
        void SetPreviewPiece(PreviewPieceIndex previewPieceIndex,
                             const Piece* pieceType,
                             Rotation rotation,
                             Rotation hudRotation);
        void ManageMoveHistory(FallingPieceSpawnReason fallingPieceSpawnReason);
        void StartBlastAreaAtGhostPiece();
        void SetBlastAreaPositionAtGhostPiece();
        void ShowFallingPiece();
        void RemoveFallingPiece();
        void ShowDraggedPiece();
        void RemoveDraggedPiece();
        void NextMove();
        void ManageBlastArea();
        void UpdateLevelProgress();
        Pht::Vec2 CalculateFallingPieceSpawnPos(const Piece& pieceType,
                                                Rotation rotation,
                                                FallingPieceSpawnReason fallingPieceSpawnReason);
        Rotation CalculateFallingPieceRotation(const Piece& pieceType,
                                               FallingPieceSpawnReason fallingPieceSpawnReason);
        void HandleCascading();
        void UpdateFieldExplosionsStates();
        void HandleSettingsChange();
        void UpdateFallingPieceYpos();
        void LandFallingPiece(bool finalMovementWasADrop);
        void DetonateDroppedBomb();
        void DetonateImpactedLevelBombs(const Field::ImpactedBombs& impactedLevelBombs);
        void GoToFieldExplosionsState();
        void RemoveClearedRowsAndPullDownLoosePieces(bool doBounceCalculations = true);
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
        bool LevelAllowsClearingFilledRows() const;
        void PlayLandPieceSound();
        void RemoveBlocksInsideTheShield();
        bool IsThereRoomToSwitchPiece(const Piece& pieceType, Rotation rotation);
        void UpdateDraggedGhostPieceRowAndBlastArea();
        const Move* GetValidMoveBelowDraggedPiece(int& ghostPieceRow);
        void CancelDraggingBecausePieceLands();
        Result HandleInput();
        void ForwardTouchToInputHandler(const Pht::TouchEvent& touchEvent);
        bool IsInputAllowed() const;
        
        enum class State {
            LogicUpdate,
            FieldExplosions
        };
        
        enum class CascadeState {
            NotCascading,
            Cascading,
            WaitingToClearLine
        };
        
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
        };
        
        Pht::IEngine& mEngine;
        Field& mField;
        ScrollController& mScrollController;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        FlashingBlocksAnimation& mFlashingBlocksAnimation;
        CollapsingFieldAnimation& mCollapsingFieldAnimation;
        EffectManager& mEffectManager;
        PieceDropParticleEffect& mPieceDropParticleEffect;
        PieceTrailParticleEffect& mPieceTrailParticleEffect;
        BlastArea& mBlastArea;
        FallingPieceScaleAnimation& mFallingPieceScaleAnimation;
        Shield& mShield;
        ValidAreaAnimation& mValidAreaAnimation;
        SmallText& mSmallText;
        GameHudController& mGameHudController;
        GameScene& mScene;
        Tutorial& mTutorial;
        const SettingsService& mSettingsService;
        ControlType mControlType;
        bool mIsSwipeGhostPieceEnabled {false};
        State mState {State::LogicUpdate};
        CascadeState mCascadeState {CascadeState::NotCascading};
        float mCascadeWaitTime {0.0f};
        FieldGravity mFieldGravity;
        FallingPiece mFallingPieceStorage;
        DraggedPiece mDraggedPieceStorage;
        FieldExplosionsStates mFieldExplosionsStates;
        FallingPieceAnimation mFallingPieceAnimation;
        DraggedPieceAnimation mDraggedPieceAnimation;
        ComboDetector mComboDetector;
        Ai mAi;
        Moves* mAllValidMoves {nullptr};
        DragInputHandler mDragInputHandler;
        GestureInputHandler mGestureInputHandler;
        ClickInputHandler mClickInputHandler;
        DraggedPiece* mDraggedPiece {nullptr};
        FallingPiece* mFallingPiece {nullptr};
        FallingPieceSpawnReason mFallingPieceSpawnReason {FallingPieceSpawnReason::None};
        const Piece* mFallingPieceSpawnType {nullptr};
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
        int mMovesUsed {0};
        int mMovesLeft {0};
        int mNumObjectsLeftToClear {0};
        bool mShouldUndoMove {false};
        int mNumUndosUsed {0};
    };
}

#endif
