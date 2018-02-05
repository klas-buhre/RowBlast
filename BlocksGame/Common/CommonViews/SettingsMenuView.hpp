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

        Pht::SceneObject& GetControlsClickText() {
            return *mControlsClickText;
        }

        Pht::SceneObject& GetControlsSwipeText() {
            return *mControlsSwipeText;
        }
        
        Pht::SceneObject& GetSoundOnText() {
            return *mSoundOnText;
        }

        Pht::SceneObject& GetSoundOffText() {
            return *mSoundOffText;
        }
        
    private:
        std::unique_ptr<MenuButton> mControlsButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mControlsClickText {nullptr};
        Pht::SceneObject* mControlsSwipeText {nullptr};
        Pht::SceneObject* mSoundOnText {nullptr};
        Pht::SceneObject* mSoundOffText {nullptr};
    };
}

#endif
