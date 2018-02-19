#ifndef FadeEffect_hpp
#define FadeEffect_hpp

#include "RenderableObject.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class ISceneManager;
    class IRenderer;
    
    class FadeEffect {
    public:
        enum class State {
            FadingOut,
            Transition,
            FadingIn,
            Idle
        };
        
        FadeEffect(ISceneManager& sceneManager,
                   IRenderer& renderer,
                   float duration,
                   float midFade,
                   float zPosition);
        
        void Reset();
        void Start();
        void StartInMidFade();
        State Update(float dt);
        bool IsFadingOut() const;
        void SetDuration(float duration);
        
        State GetState() const {
            return mState;
        }
        
        SceneObject& GetSceneObject() {
            return *mSceneObject;
        }
        
    private:
        IRenderer& mRenderer;
        State mState {State::Idle};
        std::unique_ptr<SceneObject> mSceneObject;
        std::unique_ptr<RenderableObject> mQuad;
        float mFade {0.0f};
        float mMidFade {0.0f};
        float mFadeSpeed {1.0f};
    };
}

#endif
