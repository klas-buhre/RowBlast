#ifndef NoMovesDialogView_hpp
#define NoMovesDialogView_hpp

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
    
    class NoMovesDialogView: public Pht::GuiView {
    public:
        NoMovesDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        const CloseButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetPlayOnButton() const {
            return *mPlayOnButton;
        }
        
    private:
        std::unique_ptr<CloseButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayOnButton;
    };
}

#endif
