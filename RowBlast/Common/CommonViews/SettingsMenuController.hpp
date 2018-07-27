#ifndef SettingsMenuController_hpp
#define SettingsMenuController_hpp

// Game includes.
#include "SettingsMenuView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class Settings;
    
    class SettingsMenuController {
    public:
        enum class Result {
            None,
            GoBack
        };
        
        SettingsMenuController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               Settings& settings,
                               PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void Init(SlidingMenuAnimation::UpdateFade updateFade, bool isGestureControlsAllowed);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        void UpdateViewToReflectSettings(bool isGestureControlsAllowed);
        
        Pht::IEngine& mEngine;
        Settings& mSettings;
        SettingsMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        SlidingMenuAnimation::UpdateFade mUpdateFade {SlidingMenuAnimation::UpdateFade::Yes};
    };
}

#endif
