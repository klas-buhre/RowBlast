#ifndef GestureInputHandler_hpp
#define GestureInputHandler_hpp

// Engine includes.
#include "SwipeGestureRecognizer.hpp"

namespace BlocksGame {
    class FallingPiece;
    class IGameLogic;
    class Level;
    
    class GestureInputHandler {
    public:
        GestureInputHandler(IGameLogic& gameLogic, FallingPiece& fallingPiece);
        
        void Init(const Level& level);
        void HandleTouch(const Pht::TouchEvent& touchEvent);
        
    private:
        void HandleTouchBegin();
        Pht::IsSwipe HandleSwipeDownOrUp(const Pht::TouchEvent& touchEvent);
        bool ShouldDropFallingPieceOnTouchEnd();
        bool TouchIsTap();
        bool TouchIsSwipeUp();
        void HandleDraggingDown(const Pht::TouchEvent& touchEvent);
        void HandleDraggingSideways(const Pht::TouchEvent& touchEvent);
        void CalculateXWithCollisionDetection(float fallingPieceNewX);
        void EnsureBeingDraggedDownState(const Pht::TouchEvent& touchEvent);
        void MaybeExitBeingDraggedDownState();
        void ExitBeingDraggedDownState();
        
        enum class DiffState {
            Accumulating,
            Diffing
        };
        
        IGameLogic& mGameLogic;
        FallingPiece& mFallingPiece;
        const Level* mLevel {nullptr};
        int mPieceIdAtPanBegin {0};
        float mPreviousTranslationX {0.0f};
        float mPreviousTranslationY {0.0f};
        Pht::SwipeGestureRecognizer mSwipeUpRecognizer;
        Pht::SwipeGestureRecognizer mSwipeDownRecognizer;
        DiffState mDiffState {DiffState::Accumulating};
        float mAccumulatedDiff {0.0f};
        int mLastNonZeroDiffSign {0};
        bool mTouchTriggeredDiffingState {false};
        bool mTouchTriggeredDragDownState {false};
        bool mTouchContainsSwipeUp {false};
        float mFallingPieceXAtDragDown {0.0f};
        DiffState mDiffStateAtDragDown {DiffState::Accumulating};
        bool mDragDownContainedUpMovement {false};
    };
}

#endif
