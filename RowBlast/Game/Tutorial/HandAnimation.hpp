#ifndef HandAnimation_hpp
#define Hand_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
    class ParticleEffect;
}

namespace RowBlast {
    class GameScene;

    class HandAnimation {
    public:
        HandAnimation(Pht::IEngine& engine, GameScene& scene);
        
        void Init();
        void Start(const Pht::Vec3& position, float angle);
        void Update();
        void Stop();
        
    private:
        enum class State {
            Inactive,
            GoingForward,
            GoingBackward,
            Still
        };
        
        void CreateCircleParticleEffect();
        void UpdateInGoingForwardState();
        void UpdateInGoingBackwardState();
        void UpdateInStillState();
        void GoToForwardState();
        
        Pht::IEngine& mEngine;
        GameScene& mScene;
        State mState {State::Inactive};
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mHandSceneObject {nullptr};
        Pht::ParticleEffect* mCircleEffect {nullptr};
        float mElapsedTime {0.0f};
    };
}

#endif
