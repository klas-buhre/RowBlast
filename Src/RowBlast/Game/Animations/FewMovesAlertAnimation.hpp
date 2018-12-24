#ifndef FewMovesAlertAnimation_hpp
#define FewMovesAlertAnimation_hpp

namespace Pht {
    class SceneObject;
}

namespace RowBlast {
    class GameScene;
    class GameLogic;

    class FewMovesAlertAnimation {
    public:
        FewMovesAlertAnimation(GameScene& scene, const GameLogic& gameLogic);
        
        void Init();
        void Update(float dt);
        
    private:
        void UpdateInInactiveState();
        void UpdateInActiveState(float dt);

        enum class State {
            Active,
            Inactive
        };

        GameScene& mScene;
        const GameLogic& mGameLogic;
        State mState {State::Inactive};
        Pht::SceneObject* mMovesTextContainerSceneObject {nullptr};
        float mElapsedTime {0.0f};
    };
}

#endif
