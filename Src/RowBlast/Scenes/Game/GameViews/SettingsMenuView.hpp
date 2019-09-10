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
        void SetMusicOnIsVisible(bool isVisible);
        void SetMusicOffIsVisible(bool isVisible);
        void SetSoundOnIsVisible(bool isVisible);
        void SetSoundOffIsVisible(bool isVisible);
        void SetClickControlsIsVisible(bool isVisible);
        void SetGestureControlsIsVisible(bool isVisible);
        void SetGhostPieceOnIsVisible(bool isVisible);
        void SetGhostPieceOffIsVisible(bool isVisible);

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        bool IsControlsButtonEnabled() const {
            return mIsControlsButtonEnabled;
        }

        const MenuButton& GetControlsButton() const {
            return *mControlsButton;
        }

        const MenuButton& GetGhostPieceButton() const {
            return *mGhostPieceButton;
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
        
    private:
        const CommonResources& mCommonResources;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mControlsButton;
        std::unique_ptr<MenuButton> mGhostPieceButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mMusicButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mControlsClickText {nullptr};
        Pht::SceneObject* mControlsClickIcon1 {nullptr};
        Pht::SceneObject* mControlsClickIcon2 {nullptr};
        Pht::SceneObject* mControlsClickIcon3 {nullptr};
        Pht::SceneObject* mControlsGestureText {nullptr};
        Pht::SceneObject* mControlsGestureIcon1 {nullptr};
        Pht::SceneObject* mControlsGestureIcon2 {nullptr};
        Pht::SceneObject* mControlsGestureIcon3 {nullptr};
        Pht::SceneObject* mGhostPieceDisabledIcon {nullptr};
        Pht::SceneObject* mGhostPieceOnText {nullptr};
        Pht::SceneObject* mGhostPieceOffText {nullptr};
        Pht::SceneObject* mSoundOnText {nullptr};
        Pht::SceneObject* mSoundOnIcon {nullptr};
        Pht::SceneObject* mSoundOffText {nullptr};
        Pht::SceneObject* mSoundOffIcon {nullptr};
        Pht::SceneObject* mMusicOnText {nullptr};
        Pht::SceneObject* mMusicOnIcon {nullptr};
        Pht::SceneObject* mMusicOffText {nullptr};
        Pht::SceneObject* mMusicOffIcon {nullptr};
        bool mIsControlsButtonEnabled {true};
    };
}

#endif
