#ifndef OptionsMenuController_hpp
#define OptionsMenuController_hpp

// Game includes.
#include "OptionsMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class UserServices;
    class CommonResources;
    
    class OptionsMenuController {
    public:
        enum class Result {
            None,
            GoToAboutMenu,
            GoBack
        };
        
        OptionsMenuController(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              UserServices& userServices);
        
        void SetUp();
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        void UpdateViewToReflectSettings();
        
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        OptionsMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
