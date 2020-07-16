#ifndef StoreErrorDialogView_hpp
#define StoreErrorDialogView_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class StoreErrorDialogView: public Pht::GuiView {
    public:
        StoreErrorDialogView(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             const std::string& caption,
                             const std::vector<std::string>& textLines,
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
