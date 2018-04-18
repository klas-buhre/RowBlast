#ifndef GameOverDialogView_hpp
#define GameOverDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CloseButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class GameOverDialogView: public Pht::GuiView {
    public:
        GameOverDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        const CloseButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetRetryButton() const {
            return *mRetryButton;
        }
        
    private:
        std::unique_ptr<CloseButton> mCloseButton;
        std::unique_ptr<MenuButton> mRetryButton;
    };
}

#endif
