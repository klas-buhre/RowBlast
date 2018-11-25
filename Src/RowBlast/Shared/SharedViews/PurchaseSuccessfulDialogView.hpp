#ifndef PurchaseSuccessfulDialogView_hpp
#define PurchaseSuccessfulDialogView_hpp


#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class TextComponent;
}

namespace RowBlast {
    class PurchaseSuccessfulDialogView: public Pht::GuiView {
    public:
        PurchaseSuccessfulDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     PotentiallyZoomedScreen potentiallyZoomedScreen);

        void SetUp(int numCoins);

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetOkButton() const {
            return *mOkButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mOkButton;
        Pht::TextComponent* mConfirmationText {nullptr};
    };
}

#endif
