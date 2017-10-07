#ifndef SettingsMenuController_hpp
#define SettingsMenuController_hpp

// Game includes.
#include "SettingsMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace BlocksGame {
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
        
        void Reset();
        Result Update();
        
        const Pht::GuiView& GetView() const {
            return mView;
        }
        
        const Pht::FadeEffect& GetFadeEffect() const {
            return mSlidingMenuAnimation.GetFadeEffect();
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IEngine& mEngine;
        Settings& mSettings;
        SettingsMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
