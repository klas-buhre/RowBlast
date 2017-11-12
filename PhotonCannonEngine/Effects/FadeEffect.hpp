#ifndef FadeEffect_hpp
#define FadeEffect_hpp

#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    
    class FadeEffect {
    public:
        enum class State {
            FadingOut,
            Transition,
            FadingIn,
            Idle
        };
        
        FadeEffect(IEngine& engine, float duration, float midFade);
        
        void Reset();
        void Start();
        State Update();
        State UpdateAndRender();
        void Render() const;
        bool IsFadingOut() const;
        
        State GetState() const {
            return mState;
        }
        
    private:
        IEngine& mEngine;
        State mState {State::Idle};
        std::unique_ptr<RenderableObject> mQuad;
        float mFade {0.0f};
        float mMidFade {0.0f};
        float mFadeSpeed {1.0f};
    };
}

#endif
