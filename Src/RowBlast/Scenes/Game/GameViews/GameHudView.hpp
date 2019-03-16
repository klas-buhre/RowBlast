#ifndef GameHudView_hpp
#define GameHudView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"
#include "Button.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class GameHudView: public Pht::GuiView {
    public:
        GameHudView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        Pht::Button& GetPauseButton() const {
            return *mPauseButton;
        }

        Pht::Button& GetSwitchButton() const {
            return *mSwitchButton;
        }

    private:
        std::unique_ptr<Pht::Button> mPauseButton;
        std::unique_ptr<Pht::Button> mSwitchButton;
    };
}

#endif
