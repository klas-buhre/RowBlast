#ifndef GameLogic_hpp
#define GameLogic_hpp

// Game includes.
#include "IGameLogic.hpp"
#include "FallingPiece.hpp"
#include "NextPieceGenerator.hpp"
#include "CollisionDetection.hpp"
#include "Piece.hpp"
#include "GestureInputHandler.hpp"
#include "ClickInputHandler.hpp"
#include "FallingPieceAnimation.hpp"
#include "Settings.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class Field;
    class Level;
    class ScrollController;
    class ExplosionParticleEffect;
    class RowExplosionParticleEffect;
    class FlyingBlocksAnimation;
    class PieceDropParticleEffect;
    class GameHudController;
    class IInputHandler;
    class GameScene;
    class BlastRadiusAnimation;
    
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
                  const ScrollController& scrollController,
                  const GameScene& gameScene,
                  ExplosionParticleEffect& explosionParticleEffect,
                  RowExplosionParticleEffect& rowExplosionParticleEffect,
                  FlyingBlocksAnimation& flyingBlocksAnimation,
                  PieceDropParticleEffect& pieceDropParticleEffect,
                  BlastRadiusAnimation& blastRadiusAnimation,
                  GameHudController& gameHudController,
                  const Settings& settings);
        
        void DropFallingPiece() override;
        void StartFallingPieceAnimation(const Movement& lastMovement) override;
        void OnFallingPieceAnimationFinished(bool startParticleEffect) override;
        void RotatePieceOrDetonateBomb() override;
        void SwitchPiece() override;
        void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) override;
        int GetGhostPieceRow() const override;
        void StartBlastRadiusAnimation(const Pht::IVec2& position) override;
        void StopBlastRadiusAnimation() override;
        
        void Reset(const Level& level);
        Result Update(bool shouldUpdateLogic);
        bool IsUndoMovePossible() const;
        void UndoMove();
        int GetMovesUsedIncludingCurrent() const;
        
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
        
        int GetMovesLeft() const {
            return mMovesLeft;
        }
        
        bool IsUsingClickControls() const {
            return mPreviousControlType == ControlType::Click;
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
        Result InitFallingPiece();
        void CalculatePieceType();
        bool IsLevelCompleted();
        void ManageMoveHistory();
        void StartBlastRadiusAnimationAtGhostPiece();
        void SetBlastRadiusAnimationPositionAtGhostPiece();
        void RemoveFallingPiece();
        void NextMove();
        void UpdateLevelProgress();
        Pht::Vec2 CalculateFallingPieceInitPos();
        void HandleCascading();
        void HandleControlTypeChange();
        void UpdateFallingPieceYpos();
        void LandFallingPiece(bool startParticleEffect);
        void DetonateBomb();
        void PullDownLoosePieces();
        void RotateFallingPiece();
        void RotatateAndAdjustPosition(Rotation newRotation,
                                       const PieceBlocks& pieceBlocks,
                                       const Pht::IVec2& position,
                                       Direction collisionDirection);
        Result HandleInput();
        void ForwardTouchToInputHandler(const Pht::TouchEvent& touchEvent);
        
        enum class CascadeState {
            NotCascading,
            Cascading,
            WaitingToClearLine
        };
        
        enum class FallingPieceInitReason {
            None,
            NextMove,
            Switch,
            UndoMove
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
        ExplosionParticleEffect& mExplosionParticleEffect;
        RowExplosionParticleEffect& mRowExplosionParticleEffect;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        PieceDropParticleEffect& mPieceDropParticleEffect;
        BlastRadiusAnimation& mBlastRadiusAnimation;
        GameHudController& mGameHudController;
        const Settings& mSettings;
        ControlType mPreviousControlType;
        CascadeState mCascadeState {CascadeState::NotCascading};
        float mCascadeWaitTime {0.0f};
        FallingPieceAnimation mFallingPieceAnimation;
        FallingPiece mFallingPieceStorage;
        GestureInputHandler mGestureInputHandler;
        ClickInputHandler mClickInputHandler;
        FallingPiece* mFallingPiece {nullptr};
        FallingPieceInitReason mFallingPieceInitReason {FallingPieceInitReason::None};
        const Piece* mFallingPieceInitType {nullptr};
        MoveData mCurrentMove;
        MoveData mCurrentMoveInitialState;
        MoveData mPreviousMoveInitialState;
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
