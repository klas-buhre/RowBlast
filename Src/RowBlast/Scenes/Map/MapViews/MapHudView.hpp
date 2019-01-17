#ifndef MapHudView_hpp
#define MapHudView_hpp

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
    
    class MapHudView: public Pht::GuiView {
    public:
        explicit MapHudView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetOptionsButton() const {
            return *mOptionsButton;
        }

        const MenuButton& GetCoinsButton() const {
            return *mCoinsButton;
        }

        const MenuButton& GetLivesButton() const {
            return *mLivesButton;
        }

    private:
        std::unique_ptr<MenuButton> mOptionsButton;
        std::unique_ptr<MenuButton> mCoinsButton;
        std::unique_ptr<MenuButton> mLivesButton;
    };
}

#endif
