#ifndef SettingsMenuView_hpp
#define SettingsMenuView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    
    class SettingsMenuView: public Pht::GuiView {
    public:
        SettingsMenuView(Pht::IEngine& engine, const CommonResources& commonResources);

        const MenuButton& GetControlsButton() const {
            return *mControlsButton;
        }
        
        const MenuButton& GetSoundButton() const {
            return *mSoundButton;
        }

        const MenuButton& GetBackButton() const {
            return *mBackButton;
        }

        Pht::Text& GetControlsClickText() {
            return *mControlsClickText;
        }

        Pht::Text& GetControlsSwipeText() {
            return *mControlsSwipeText;
        }
        
        Pht::Text& GetSoundOnText() {
            return *mSoundOnText;
        }

        Pht::Text& GetSoundOffText() {
            return *mSoundOffText;
        }
        
    private:
        std::unique_ptr<MenuButton> mControlsButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::Text* mControlsClickText {nullptr};
        Pht::Text* mControlsSwipeText {nullptr};
        Pht::Text* mSoundOnText {nullptr};
        Pht::Text* mSoundOffText {nullptr};
    };
}

#endif
