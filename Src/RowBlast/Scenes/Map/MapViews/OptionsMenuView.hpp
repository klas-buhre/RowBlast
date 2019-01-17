#ifndef OptionsMenuView_hpp
#define OptionsMenuView_hpp

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

    class OptionsMenuView: public Pht::GuiView {
    public:
        OptionsMenuView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
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

        const MenuButton& GetHowToPlayButton() const {
            return *mHowToPlayButton;
        }

        const MenuButton& GetAboutButton() const {
            return *mAboutButton;
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
        std::unique_ptr<MenuButton> mHowToPlayButton;
        std::unique_ptr<MenuButton> mAboutButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mControlsClickText {nullptr};
        Pht::SceneObject* mControlsSwipeText {nullptr};
        Pht::SceneObject* mSoundOnText {nullptr};
        Pht::SceneObject* mSoundOffText {nullptr};
        Pht::SceneObject* mMusicOnText {nullptr};
        Pht::SceneObject* mMusicOffText {nullptr};
    };
}

#endif
