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
            Dragging,
            DragEnd,
            Idle
        };
        
        DragInputHandler(Pht::IEngine& engine,
                         IGameLogic& gameLogic,
                         GameScene& gameScene,
                         DraggedPiece& draggedPiece);
        
        void Init();
        State HandleTouch(const Pht::TouchEvent& touchEvent);
        
    private:
        void HandleTouchBegin(const Pht::TouchEvent& touchEvent);
        void HandleOngoingTouch(const Pht::TouchEvent& touchEvent);
        void HandleTouchEnd(const Pht::TouchEvent& touchEvent);
        bool TryBeginDrag(DraggedPieceIndex draggedPieceIndex, const Pht::TouchEvent& touchEvent);
        Pht::Vec2 CalculatePiecePosition(const Pht::TouchEvent& touchEvent) const;
        const Piece* GetPieceType(DraggedPieceIndex draggedPieceIndex) const;
        Rotation GetPieceRotation(DraggedPieceIndex draggedPieceIndex) const;
        Pht::Button& GetPreviewPieceButton(DraggedPieceIndex draggedPieceIndex) const;
        Pht::SceneObject* GetPreviewPieceSceneObject(DraggedPieceIndex draggedPieceIndex) const;
        
        Pht::IEngine& mEngine;
        IGameLogic& mGameLogic;
        GameScene& mScene;
        DraggedPiece& mDraggedPiece;
        State mState {State::Idle};
        DraggedPieceIndex mDraggedPieceIndex {DraggedPieceIndex::None};
    };
}

#endif
