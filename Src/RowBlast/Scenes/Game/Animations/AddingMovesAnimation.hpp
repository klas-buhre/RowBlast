#ifndef AddingMovesAnimation_hpp
#define AddingMovesAnimation_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
}

namespace RowBlast {
    class GameScene;

    class AddingMovesAnimation {
    public:
        AddingMovesAnimation(Pht::IEngine& engine, GameScene& scene);
        
        void Init();
        void Start();
        void Update(float dt);
        
    private:
        void CreateParticleEffect(Pht::IEngine& engine);
        void UpdateInActiveState(float dt);
        void GoToInactiveState();

        enum class State {
            Active,
            Inactive
        };

        Pht::IAudio& mAudio;
        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        int mMovesLeft {0};
        std::unique_ptr<Pht::SceneObject> mParticleEffect;
    };
}

#endif
