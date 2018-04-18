#ifndef MapConfirmationDialogView_hpp
#define MapConfirmationDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class MapConfirmationDialogView: public Pht::GuiView {
    public:
        MapConfirmationDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        const MenuButton& GetYesButton() const {
            return *mYesButton;
        }
        
        const MenuButton& GetNoButton() const {
            return *mNoButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mYesButton;
        std::unique_ptr<MenuButton> mNoButton;
    };
}

#endif
