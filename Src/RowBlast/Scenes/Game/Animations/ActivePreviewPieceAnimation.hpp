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
        void GoToInactiveState();

        enum class State {
            Active,
            Inactive
        };
        
        GameScene& mScene;
        GameLogic& mGameLogic;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
    };
}

#endif
