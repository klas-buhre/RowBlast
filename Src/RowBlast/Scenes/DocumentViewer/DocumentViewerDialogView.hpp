#ifndef DocumentViewerDialogView_hpp
#define DocumentViewerDialogView_hpp

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

    class DocumentViewerDialogView: public Pht::GuiView {
    public:
        DocumentViewerDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
    
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }

        const MenuButton& GetBackButton() const {
            return *mBackButton;
        }

    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mBackButton;
    };
}
        
#endif
