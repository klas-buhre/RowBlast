#ifndef LevelCompletedDialogView_hpp
#define LevelCompletedDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class LevelCompletedDialogView: public Pht::GuiView {
    public:
        LevelCompletedDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetNextButton() const {
            return *mNextButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mNextButton;
    };
}

#endif
