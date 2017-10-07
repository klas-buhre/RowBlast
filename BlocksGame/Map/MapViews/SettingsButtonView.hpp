#ifndef SettingsButtonView_hpp
#define SettingsButtonView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"
#include "Button.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class SettingsButtonView: public Pht::GuiView {
    public:
        explicit SettingsButtonView(Pht::IEngine& engine);
        
        Pht::Button& GetButton() const {
            return *mButton;
        }
        
    private:
        std::unique_ptr<Pht::Button> mButton;
    };
}

#endif
