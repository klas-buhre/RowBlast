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
        
        void SetMusicOnIsVisible(bool isVisible);
        void SetMusicOffIsVisible(bool isVisible);
        void SetSoundOnIsVisible(bool isVisible);
        void SetSoundOffIsVisible(bool isVisible);
        void SetControlsClickIsVisible(bool isVisible);
        void SetControlsSwipeIsVisible(bool isVisible);

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

    private:
        void CreateIcon(const std::string& filename,
                        const Pht::Vec3& position,
                        const Pht::Vec2& size,
                        Pht::IEngine& engine);
        
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mControlsButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mMusicButton;
        std::unique_ptr<MenuButton> mHowToPlayButton;
        std::unique_ptr<MenuButton> mAboutButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mControlsClickText {nullptr};
        Pht::SceneObject* mControlsClickIcon1 {nullptr};
        Pht::SceneObject* mControlsClickIcon2 {nullptr};
        Pht::SceneObject* mControlsClickIcon3 {nullptr};
        Pht::SceneObject* mControlsSwipeText {nullptr};
        Pht::SceneObject* mControlsSwipeIcon1 {nullptr};
        Pht::SceneObject* mControlsSwipeIcon2 {nullptr};
        Pht::SceneObject* mControlsSwipeIcon3 {nullptr};
        Pht::SceneObject* mSoundOnText {nullptr};
        Pht::SceneObject* mSoundOnIcon {nullptr};
        Pht::SceneObject* mSoundOffText {nullptr};
        Pht::SceneObject* mSoundOffIcon {nullptr};
        Pht::SceneObject* mMusicOnText {nullptr};
        Pht::SceneObject* mMusicOnIcon {nullptr};
        Pht::SceneObject* mMusicOffText {nullptr};
        Pht::SceneObject* mMusicOffIcon {nullptr};
    };
}

#endif
