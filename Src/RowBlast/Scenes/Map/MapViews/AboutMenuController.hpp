#ifndef AboutMenuController_hpp
#define AboutMenuController_hpp

// Game includes.
#include "AboutMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IEngine;
    class IInput;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class CommonResources;
    
    class AboutMenuController {
    public:
        enum class Result {
            None,
            ViewTermsOfService,
            ViewPrivacyPolicy,
            ViewCredits,
            GoBack
        };
        
        AboutMenuController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFade);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        AboutMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
