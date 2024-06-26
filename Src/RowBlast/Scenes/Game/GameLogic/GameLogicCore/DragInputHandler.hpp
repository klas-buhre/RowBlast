#ifndef DragInputHandler_hpp
#define DragInputHandler_hpp

// Game includes.
#include "Cell.hpp"
#include "IGameLogic.hpp"

namespace Pht {
    class TouchEvent;
    class IEngine;
    class SceneObject;
    class Button;
}

namespace RowBlast {
    class IGameLogic;
    class GameScene;
    class DraggedPiece;
    class Piece;
    
    class DragInputHandler {
    public:
        enum class State {
            Idle,
            TouchingPreviewPieceButton,
            Dragging,
            DragEnd
        };
        
        DragInputHandler(Pht::IEngine& engine,
                         IGameLogic& gameLogic,
                         GameScene& gameScene,
                         DraggedPiece& draggedPiece);
        
        void Init();
        State HandleTouch(const Pht::TouchEvent& touchEvent);
        void EndDrag();
        
    private:
        void HandleTouchBegin(const Pht::TouchEvent& touchEvent);
        void HandleOngoingTouch(const Pht::TouchEvent& touchEvent);
        void HandleTouchEnd(const Pht::TouchEvent& touchEvent);
        void HandleTouchBeginInIdleState(const Pht::TouchEvent& touchEvent);
        void HandleOngoingTouchInTouchingPreviewPieceButtonState(const Pht::TouchEvent& touchEvent);
        void HandleOngoingTouchInDraggingState(const Pht::TouchEvent& touchEvent);
        void HandleTouchEndInTouchingPreviewPieceButtonState(const Pht::TouchEvent& touchEvent);
        void HandleTouchEndInDraggingState(const Pht::TouchEvent& touchEvent);
        void HandleTouchCancelled();
        bool IsTouchDistanceLongEnoughForDrag(const Pht::TouchEvent& touchEvent);
        void NotifyGameLogicIfPieceMovedGridPosition();
        void UpdatePiecePosition(const Pht::TouchEvent& touchEvent);
        PreviewPieceIndex CalculateDraggedPieceIndexAtTouchBegin(const Pht::TouchEvent& touchEvent);
        const Piece* GetPieceType(PreviewPieceIndex draggedPieceIndex) const;
        Rotation GetPieceRotation(PreviewPieceIndex draggedPieceIndex) const;
        Pht::Button& GetPreviewPieceButton(PreviewPieceIndex draggedPieceIndex) const;
        
        Pht::IEngine& mEngine;
        IGameLogic& mGameLogic;
        GameScene& mScene;
        DraggedPiece& mDraggedPiece;
        State mState {State::Idle};
        Pht::IVec2 mPreviousGridPosition;
        PreviewPieceIndex mDraggedPieceIndex {PreviewPieceIndex::None};
    };
}

#endif
