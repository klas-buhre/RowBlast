#ifndef GameHudController_hpp
#define GameHudController_hpp

// Game includes.
#include "GameHudView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace RowBlast {
    class IGameHudEventListener;
    class CommonResources;
    
    class GameHudController {
    public:
        enum class Result {
            None,
            TouchStartedOverButton,
            ClickedSwitch,
            ClickedPause
        };
        
        GameHudController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        Result OnTouch(const Pht::TouchEvent& event, bool isSwitchButtonEnabled);
        
        void SetHudEventListener(IGameHudEventListener& gameHudEventListener) {
            mGameHudEventListener = &gameHudEventListener;
        }
        
        Pht::GuiView& GetView() {
            return mView;
        }
    
    private:
        bool IsSwitchButtonClicked(const Pht::TouchEvent& event) const;
        
        GameHudView mView;
        Pht::IEngine& mEngine;
        IGameHudEventListener* mGameHudEventListener {nullptr};
    };
}

#endif
