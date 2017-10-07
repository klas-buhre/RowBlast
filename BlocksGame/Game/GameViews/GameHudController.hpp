#ifndef GameHudController_hpp
#define GameHudController_hpp

// Game includes.
#include "GameHudView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace BlocksGame {
    class IGameHudEventListener;
    
    class GameHudController {
    public:
        enum class Result {
            None,
            TouchStartedOverButton,
            ClickedSwitch,
            ClickedPause
        };
        
        GameHudController(Pht::IEngine& engine);
        
        Result OnTouch(const Pht::TouchEvent& event);
        
        void SetHudEventListener(IGameHudEventListener& gameHudEventListener) {
            mGameHudEventListener = &gameHudEventListener;
        }
        
        const Pht::GuiView& GetView() const {
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
