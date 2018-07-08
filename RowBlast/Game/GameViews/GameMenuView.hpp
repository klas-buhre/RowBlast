#ifndef GameMenuView_hpp
#define GameMenuView_hpp

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
    
    class GameMenuView: public Pht::GuiView {
    public:
        GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void EnableUndoButton();
        void DisableUndoButton();
        
        bool IsUndoButtonEnabled() const {
            return mIsUndoButtonEnabled;
        }
        
        const MenuButton& GetResumeButton() const {
            return *mResumeButton;
        }

        const MenuButton& GetUndoButton() const {
            return *mUndoButton;
        }
        
        const MenuButton& GetGoalButton() const {
            return *mGoalButton;
        }

        const MenuButton& GetRestartButton() const {
            return *mRestartButton;
        }

        const MenuButton& GetSettingsButton() const {
            return *mSettingsButton;
        }

        const MenuButton& GetMapButton() const {
            return *mMapButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mResumeButton;
        std::unique_ptr<MenuButton> mUndoButton;
        std::unique_ptr<MenuButton> mGoalButton;
        std::unique_ptr<MenuButton> mRestartButton;
        std::unique_ptr<MenuButton> mSettingsButton;
        std::unique_ptr<MenuButton> mMapButton;
        bool mIsUndoButtonEnabled {true};
    };
}

#endif
