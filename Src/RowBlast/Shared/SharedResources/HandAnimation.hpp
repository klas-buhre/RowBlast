#ifndef HandAnimation_hpp
#define HandAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class ParticleEffect;
}

namespace RowBlast {
    class GameScene;

    class HandAnimation {
    public:
        HandAnimation(Pht::IEngine& engine);
        
        void Init(Pht::Scene& scene, Pht::SceneObject& parentObject, float scale, bool useShadow);
        void Start(const Pht::Vec3& position, float angle);
        void Update();
        void Stop();
        void Hide();
        void Unhide();
        
    private:
        enum class State {
            Inactive,
            GoingForward,
            GoingBackward,
            Still
        };
        
        void CreateCircleParticleEffect(Pht::Scene& scene);
        void UpdateInGoingForwardState();
        void UpdateInGoingBackwardState();
        void UpdateInStillState();
        void GoToForwardState();
        
        Pht::IEngine& mEngine;
        State mState {State::Inactive};
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mHandSceneObject {nullptr};
        Pht::SceneObject* mHandShadowSceneObject {nullptr};
        Pht::ParticleEffect* mCircleEffect {nullptr};
        Pht::Vec3 mHandShadowBackwardPosition;
        Pht::Vec3 mHandShadowForwardPosition;
        float mElapsedTime {0.0f};
    };
}

#endif
