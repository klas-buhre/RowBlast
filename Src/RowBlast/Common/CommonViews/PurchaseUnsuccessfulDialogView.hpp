#ifndef PurchaseUnsuccessfulDialogView_hpp
#define PurchaseUnsuccessfulDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class PurchaseUnsuccessfulDialogView: public Pht::GuiView {
    public:
        PurchaseUnsuccessfulDialogView(Pht::IEngine& engine,
                                       const CommonResources& commonResources,
                                       PotentiallyZoomedScreen potentiallyZoomedScreen);

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
