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
    class IGuiLightProvider;
    
    class MapConfirmationDialogView: public Pht::GuiView {
    public:
        MapConfirmationDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        void SetUp();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }

        const MenuButton& GetYesButton() const {
            return *mYesButton;
        }
        
        const MenuButton& GetNoButton() const {
            return *mNoButton;
        }
        
    private:
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mYesButton;
        std::unique_ptr<MenuButton> mNoButton;
    };
}

#endif
