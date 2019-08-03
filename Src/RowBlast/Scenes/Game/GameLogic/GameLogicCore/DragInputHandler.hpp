#ifndef DragInputHandler_hpp
#define DragInputHandler_hpp

namespace Pht {
    class TouchEvent;
}

namespace RowBlast {
    class IGameLogic;
    class GameScene;
    class DraggedPiece;
    
    class DragInputHandler {
    public:
        enum class State {
            Dragging,
            Idle
        };
        
        DragInputHandler(IGameLogic& gameLogic, GameScene& gameScene, DraggedPiece& draggedPiece);
        
        void Init();
        State HandleTouch(const Pht::TouchEvent& touchEvent);
        
    private:
        IGameLogic& mGameLogic;
        GameScene& mScene;
        DraggedPiece& mDraggedPiece;
        State mState {State::Idle};
    };
}

#endif
