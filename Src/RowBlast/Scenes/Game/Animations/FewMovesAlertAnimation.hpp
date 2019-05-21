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
        void UpdateInZeroMovesState();
        void RestoreHud();

        enum class State {
            Active,
            ZeroMoves,
            Inactive
        };

        GameScene& mScene;
        const GameLogic& mGameLogic;
        State mState {State::Inactive};
        Pht::SceneObject* mMovesRoundedCylinderContainer {nullptr};
        Pht::SceneObject* mMovesTextContainerSceneObject {nullptr};
        Pht::SceneObject* mMovesTextSceneObject {nullptr};
        float mElapsedTime {0.0f};
    };
}

#endif
