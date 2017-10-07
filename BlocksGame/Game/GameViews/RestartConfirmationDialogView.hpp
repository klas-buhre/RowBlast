#ifndef RestartConfirmationDialogView_hpp
#define RestartConfirmationDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    
    class RestartConfirmationDialogView: public Pht::GuiView {
    public:
        RestartConfirmationDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        const MenuButton& GetYesButton() const {
            return *mYesButton;
        }
        
        const MenuButton& GetNoButton() const {
            return *mNoButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mYesButton;
        std::unique_ptr<MenuButton> mNoButton;
        Pht::TextProperties mTextProperties;
        Pht::TextProperties mRestartTextProperties;
    };
}

#endif
