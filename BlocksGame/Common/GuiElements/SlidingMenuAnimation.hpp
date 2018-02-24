#ifndef SlidingMenuAnimation_hpp
#define SlidingMenuAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
    class FadeEffect;
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
        
        enum class UpdateFade {
            Yes,
            No
        };
        
        enum class SlideDirection {
            Up,
            Down
        };
        
        SlidingMenuAnimation(Pht::IEngine& engine, Pht::GuiView& view);
        
        void Init(UpdateFade updateFade, SlideDirection slideInDirection = SlideDirection::Up);
        void StartSlideIn();
        void StartSlideOut(UpdateFade updateFade,
                           SlideDirection slideOutDirection = SlideDirection::Down);
        State Update();

        void SetFadeEffect(Pht::FadeEffect& fadeEffect) {
            mFadeEffect = &fadeEffect;
        }
        
    private:
        void UpdateInSlidingInState();
        bool HasCompletelySlidIn(const Pht::Vec2& position);
        void GoToShowingMenuState();
        void UpdateInSlidingOutState();
        bool HasCompletelySlidOut(const Pht::Vec2& position);
        void GoToDoneState();
    
        Pht::IEngine& mEngine;
        Pht::GuiView& mView;
        State mState {State::Idle};
        SlideDirection mSlideInDirection {SlideDirection::Up};
        SlideDirection mSlideOutDirection {SlideDirection::Down};
        UpdateFade mUpdateFade {UpdateFade::Yes};
        float mElapsedTime {0.0f};
        Pht::Vec2 mVelocity;
        Pht::Vec2 mAcceleration;
        Pht::Vec2 mSlideInStartPosition;
        Pht::Vec2 mSlideOutFinalPosition;
        Pht::FadeEffect* mFadeEffect {nullptr};
    };
}

#endif
