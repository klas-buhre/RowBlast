#ifndef GameLogic_hpp
#define GameLogic_hpp

// Game includes.
#include "IGameLogic.hpp"
#include "FallingPiece.hpp"
#include "NextPieceGenerator.hpp"
#include "CollisionDetection.hpp"
#include "Piece.hpp"
#include "Field.hpp"
#include "GestureInputHandler.hpp"
#include "ClickInputHandler.hpp"
#include "FallingPieceAnimation.hpp"
#include "Settings.hpp"
#include "FieldExplosionsStates.hpp"
#include "BlastRadiusAnimation.hpp"

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
    class GameHudController;
    class IInputHandler;
    class GameScene;
    class Tutorial;
    class FlashingBlocksAnimation;
    
    enum class PreviewPieceAnimationToStart {
        NextPieceAndSwitch,
        SwitchPiece,
        RemoveActivePiece,
        None
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
                  const GameScene& gameScene,
                  EffectManager& effectManger,
                  FlyingBlocksAnimation& flyingBlocksAnimation,
                  FlashingBlocksAnimation& flashingBlocksAnimation,
                  CollapsingFieldAnimation& collapsingFieldAnimation,
                  PieceDropParticleEffect& pieceDropParticleEffect,
                  BlastRadiusAnimation& blastRadiusAnimation,
                  GameHudController& gameHudController,
                  Tutorial& tutorial,
                  const Settings& settings);
        
        void DropFallingPiece() override;
        void SelectMove(const Move& move) override;
        void OnFallingPieceAnimationFinished(bool finalMovementWasADrop) override;
        void RotatePiece(const Pht::TouchEvent& touchEvent) override;
        void SwitchPiece() override;
        void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) override;
        int GetGhostPieceRow() const override;
        bool IsInFieldExplosionsState() const override;
        void StartBlastRadiusAnimation(const Pht::IVec2& position) override;
        void StopBlastRadiusAnimation() override;
        
        void Init(const Level& level);
        Result Update(bool shouldUpdateLogic);
        bool IsUndoMovePossible() const;
        void UndoMove();
        int GetMovesUsedIncludingCurrent() const;
        const Piece* GetPieceType() const;
        
        void SetMovesLeft(int movesLeft) {
            mMovesLeft = movesLeft;
        }
        
        FallingPieceAnimation& GetFallingPieceAnimation() {
            return mFallingPieceAnimation;
        }
        
        const FallingPiece* GetFallingPiece() const {
            return mFallingPiece;
        }
        
        const NextPieceGenerator& GetNextPieceGenerator() const {
            return mCurrentMove.mNextPieceGenerator;
        }
        
        const TwoPieces& GetSelectablePieces() const {
            return mCurrentMove.mSelectablePieces;
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
        
        int GetNumLevelBlocksLeft() const {
            return mNumLevelBlocksLeft;
        }

        int GetNumEmptyBlueprintSlotsLeft() const {
            return mNumEmptyBlueprintSlotsLeft;
        }
        
    private:
        enum class FallingPieceSpawnReason {
            None,
            NextMove,
            Switch,
            UndoMove
        };

        Result SpawnFallingPiece();
        void SetPieceType();
        bool IsLevelCompleted();
        void ManageMoveHistory();
        void StartBlastRadiusAnimationAtGhostPiece();
        void SetBlastRadiusAnimationPositionAtGhostPiece();
        void RemoveFallingPiece();
        void NextMove();
        void UpdateLevelProgress();
        Pht::Vec2 CalculateFallingPieceSpawnPos(const Piece& pieceType,
                                                FallingPieceSpawnReason fallingPieceSpawnReason);
        void HandleCascading();
        void UpdateFieldExplosionsStates();
        void HandleControlTypeChange();
        void UpdateFallingPieceYpos();
        void LandFallingPiece(bool finalMovementWasADrop);
        void DetonateDroppedBomb();
        void DetonateImpactedLevelBombs(const Field::ImpactedBombs& impactedLevelBombs);
        void GoToFieldExplosionsState();
        void RemoveClearedRowsAndPullDownLoosePieces();
        Rotation CalculateNewRotation(const Pht::TouchEvent& touchEvent);
        void RotatateAndAdjustPosition(Rotation newRotation,
                                       const PieceBlocks& pieceBlocks,
                                       const Pht::IVec2& position,
                                       Direction collisionDirection);
        BlastRadiusAnimation::Kind CalculateBlastRadiusKind(const Pht::IVec2& position);
        bool IsThereRoomToSwitchPiece();
        Result HandleInput();
        void ForwardTouchToInputHandler(const Pht::TouchEvent& touchEvent);
        
        enum class State {
            LogicUpdate,
            FieldExplosions
        };
        
        enum class CascadeState {
            NotCascading,
            Cascading,
            WaitingToClearLine
        };
        
        struct MoveData {
            NextPieceGenerator mNextPieceGenerator;
            const Piece* mPieceType {nullptr};
            TwoPieces mSelectablePieces;
            int mId {0};
        };
        
        Pht::IEngine& mEngine;
        Field& mField;
        const ScrollController& mScrollController;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        FlashingBlocksAnimation& mFlashingBlocksAnimation;
        CollapsingFieldAnimation& mCollapsingFieldAnimation;
        EffectManager& mEffectManager;
        PieceDropParticleEffect& mPieceDropParticleEffect;
        BlastRadiusAnimation& mBlastRadiusAnimation;
        GameHudController& mGameHudController;
        Tutorial& mTutorial;
        const Settings& mSettings;
        ControlType mControlType;
        State mState {State::LogicUpdate};
        CascadeState mCascadeState {CascadeState::NotCascading};
        float mCascadeWaitTime {0.0f};
        FieldExplosionsStates mFieldExplosionsStates;
        FallingPieceAnimation mFallingPieceAnimation;
        FallingPiece mFallingPieceStorage;
        GestureInputHandler mGestureInputHandler;
        ClickInputHandler mClickInputHandler;
        FallingPiece* mFallingPiece {nullptr};
        FallingPieceSpawnReason mFallingPieceSpawnReason {FallingPieceSpawnReason::None};
        const Piece* mFallingPieceSpawnType {nullptr};
        MoveData mCurrentMove;
        MoveData mCurrentMoveInitialState;
        MoveData mPreviousMoveInitialState;
        PreviewPieceAnimationToStart mPreviewPieceAnimationToStart {PreviewPieceAnimationToStart::None};
        const Level* mLevel {nullptr};
        IInputHandler* mInputHandler {nullptr};
        float mLandingNoMovementDuration {0.0f};
        float mLandingMovementDuration {0.0f};
        int mGhostPieceRow {0};
        int mMovesUsed {0};
        int mMovesLeft {0};
        int mNumLevelBlocksLeft {0};
        int mNumEmptyBlueprintSlotsLeft {0};
    };
}

#endif
