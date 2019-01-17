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

namespace RowBlast {
    class CommonResources;

    class SettingsMenuView: public Pht::GuiView {
    public:
        SettingsMenuView(Pht::IEngine& engine, const CommonResources& commonResources);

        void EnableControlsButton();
        void DisableControlsButton();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        bool IsControlsButtonEnabled() const {
            return mIsControlsButtonEnabled;
        }

        const MenuButton& GetControlsButton() const {
            return *mControlsButton;
        }
        
        const MenuButton& GetSoundButton() const {
            return *mSoundButton;
        }

        const MenuButton& GetMusicButton() const {
            return *mMusicButton;
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

        Pht::SceneObject& GetMusicOnText() {
            return *mMusicOnText;
        }

        Pht::SceneObject& GetMusicOffText() {
            return *mMusicOffText;
        }

    private:
        const CommonResources& mCommonResources;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mControlsButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mMusicButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mControlsClickText {nullptr};
        Pht::SceneObject* mControlsSwipeText {nullptr};
        Pht::SceneObject* mSoundOnText {nullptr};
        Pht::SceneObject* mSoundOffText {nullptr};
        Pht::SceneObject* mMusicOnText {nullptr};
        Pht::SceneObject* mMusicOffText {nullptr};
        bool mIsControlsButtonEnabled {true};
    };
}

#endif
