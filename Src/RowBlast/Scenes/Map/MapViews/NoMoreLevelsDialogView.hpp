#ifndef NoMoreLevelsDialogView_hpp
#define NoMoreLevelsDialogView_hpp

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
    
    class NoMoreLevelsDialogView: public Pht::GuiView {
    public:
        NoMoreLevelsDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetOkButton() const {
            return *mOkButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mOkButton;
    };
}

#endif
