#ifndef PurchaseFailedDialogView_hpp
#define PurchaseFailedDialogView_hpp

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
    class PurchaseFailedDialogView: public Pht::GuiView {
    public:
        PurchaseFailedDialogView(Pht::IEngine& engine,
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