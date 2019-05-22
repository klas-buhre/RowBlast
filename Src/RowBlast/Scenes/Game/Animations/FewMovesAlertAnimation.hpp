#ifndef FewMovesAlertAnimation_hpp
#define FewMovesAlertAnimation_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class GameScene;
    class GameLogic;

    class FewMovesAlertAnimation {
    public:
        FewMovesAlertAnimation(Pht::IEngine& engine, GameScene& scene, const GameLogic& gameLogic);
        
        void Init();
        void Update(float dt);
        
    private:
        void CreateParticleEffect(Pht::IEngine& engine);
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
        std::unique_ptr<Pht::SceneObject> mParticleEffect;
    };
}

#endif
