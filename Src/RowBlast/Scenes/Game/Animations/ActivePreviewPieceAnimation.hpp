#ifndef ActivePreviewPieceAnimation_hpp
#define ActivePreviewPieceAnimation_hpp

namespace RowBlast {
    class GameScene;
    class GameLogic;
    
    class ActivePreviewPieceAnimation {
    public:
        ActivePreviewPieceAnimation(GameScene& scene, GameLogic& gameLogic);
        
        void Init();
        void Start();
        void Update(float dt);
        
    private:
        void UpdateInActiveState(float dt);
        void UpdateInStoppedDuringPieceDragState();
        void GoToInactiveState();
        void GoToStoppedDuringPieceDragState();

        enum class State {
            Active,
            StoppedDuringPieceDrag,
            Inactive
        };
        
        GameScene& mScene;
        GameLogic& mGameLogic;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
    };
}

#endif
