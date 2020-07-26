#ifndef HandAnimation_hpp
#define HandAnimation_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class GameScene;

    class HandAnimation {
    public:
        HandAnimation(Pht::IEngine& engine, float scale, bool useShadow);
        
        void Init(Pht::SceneObject& parentObject);
        void Start(const Pht::Vec3& position, float angle);
        void StartInNotTouchingScreenState(const Pht::Vec3& position,
                                           float angle,
                                           float notTouchingDuration);
        void BeginTouch(float touchDuration);
        void Update();
        void Stop();
        void Hide();
        void Unhide();
        void SetOpacity(float opacity);
        
        Pht::SceneObject& GetSceneObject() {
            return *mContainerSceneObject;
        }
        
    private:
        enum class State {
            Inactive,
            GoingForwardToScreen,
            TouchingScreen,
            GoingBackwardFromScreen,
            NotTouchingScreen
        };
        
        void CreateCircleParticleEffect();
        void Start(const Pht::Vec3& position, float angle, State initialState);
        void UpdateInGoingForwardState();
        void UpdateInTouchingScreenState();
        void UpdateInGoingBackwardState();
        void UpdateInNotTouchingScreenState();
        void SetHandAtBackwardPosition();
        
        Pht::IEngine& mEngine;
        State mState {State::Inactive};
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        std::unique_ptr<Pht::SceneObject> mHandSceneObject;
        std::unique_ptr<Pht::SceneObject> mHandShadowSceneObject;
        std::unique_ptr<Pht::SceneObject> mCircleEffect;
        Pht::SceneResources mSceneResources;
        Pht::Vec3 mHandShadowBackwardPosition;
        Pht::Vec3 mHandShadowForwardPosition;
        float mElapsedTime {0.0f};
        float mTouchDuration {0.0f};
        float mNotTouchingDuration {1.0f};
    };
}

#endif
