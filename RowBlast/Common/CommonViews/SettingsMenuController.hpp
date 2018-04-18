#ifndef SettingsMenuController_hpp
#define SettingsMenuController_hpp

// Game includes.
#include "SettingsMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class CommonResources;
    class Settings;
    
    class SettingsMenuController {
    public:
        enum class Result {
            None,
            GoBack
        };
        
        SettingsMenuController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               Settings& settings);
        
        void Init(SlidingMenuAnimation::UpdateFade updateFade);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IEngine& mEngine;
        Settings& mSettings;
        SettingsMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        SlidingMenuAnimation::UpdateFade mUpdateFade {SlidingMenuAnimation::UpdateFade::Yes};
    };
}

#endif
