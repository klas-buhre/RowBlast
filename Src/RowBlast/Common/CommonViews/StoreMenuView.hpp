#ifndef StoreMenuView_hpp
#define StoreMenuView_hpp

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
    class StoreMenuView: public Pht::GuiView {
    public:
        StoreMenuView(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
    };
}

#endif
