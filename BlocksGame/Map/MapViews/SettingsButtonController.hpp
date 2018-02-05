#ifndef SettingsButtonController_hpp
#define SettingsButtonController_hpp

// Game includes.
#include "SettingsButtonView.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
    class TouchEvent;
}

namespace BlocksGame {
    class SettingsButtonController {
    public:
        enum class Result {
            None,
            TouchStartedOverButton,
            ClickedSettings
        };
        
        explicit SettingsButtonController(Pht::IEngine& engine);
        
        Result OnTouch(const Pht::TouchEvent& event);
        
        const Pht::GuiView& GetView() const {
            return mView;
        }

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        SettingsButtonView mView;
        Pht::IAudio& mAudio;
    };
}

#endif
