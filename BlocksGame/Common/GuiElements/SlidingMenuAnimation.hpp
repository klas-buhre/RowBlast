#ifndef SlidingMenuAnimation_hpp
#define SlidingMenuAnimation_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
}

namespace BlocksGame {
    class SlidingMenuAnimation {
    public:
        enum class State {
            Idle,
            SlidingIn,
            SlidingOut,
            ShowingMenu,
            Done
        };
        
        SlidingMenuAnimation(Pht::IEngine& engine, Pht::GuiView& view, float fade);
        
        void Reset();
        void StartSlideIn();
        void StartSlideOut();
        State Update();
                
        const Pht::FadeEffect& GetFadeEffect() const {
            return mFadeEffect;
        }
        
    private:
        void UpdateInSlidingInState();
        void UpdateInSlidingOutState();
    
        Pht::IEngine& mEngine;
        Pht::GuiView& mView;
        State mState {State::Idle};
        float mElapsedTime {0.0f};
        Pht::Vec2 mVelocity;
        Pht::Vec2 mAcceleration;
        Pht::Vec2 mSlideInStartPosition;
        Pht::Vec2 mSlideOutFinalPosition;
        Pht::FadeEffect mFadeEffect;
    };
}

#endif
