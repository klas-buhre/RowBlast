#ifndef MapHudController_hpp
#define MapHudController_hpp

// Game includes.
#include "MapHudView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace RowBlast {
    class MapHudController {
    public:
        enum class Result {
            None,
            TouchStartedOverButton,
            ClickedOptionsButton,
            ClickedCoinsButton,
            ClickedLivesButton
        };
        
        MapHudController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        Result OnTouch(const Pht::TouchEvent& event);
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        MapHudView mView;
    };
}

#endif
