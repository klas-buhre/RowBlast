#ifndef StoreMenuView_hpp
#define StoreMenuView_hpp

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
    class StoreMenuView: public Pht::GuiView {
    public:
        StoreMenuView(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const std::vector<std::unique_ptr<MenuButton>>& GetProductButtons() const {
            return mProductButtons;
        }
        
    private:
        void CreateProduct(const Pht::Vec3& position,
                           const std::string& numCoins,
                           const std::string& price,
                           Pht::IEngine& engine,
                           const CommonResources& commonResources,
                           PotentiallyZoomedScreen zoom);
        
        std::unique_ptr<MenuButton> mCloseButton;
        std::vector<std::unique_ptr<MenuButton>> mProductButtons;
    };
}

#endif
