#ifndef DragInputHandler_hpp
#define DragInputHandler_hpp

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
        enum class DraggedPieceIndex {
            Active,
            Selectable0,
            Selectable1,
            None
        };

        void HandleTouchBegin(const Pht::TouchEvent& touchEvent);
        void HandleOngoingTouch(const Pht::TouchEvent& touchEvent);
        void HandleTouchEnd(const Pht::TouchEvent& touchEvent);
        bool TryBeginDrag(DraggedPieceIndex draggedPieceIndex, const Pht::TouchEvent& touchEvent);
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
