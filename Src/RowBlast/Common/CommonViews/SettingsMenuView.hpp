#ifndef SettingsMenuView_hpp
#define SettingsMenuView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "RadioButton.hpp"
#include "MenuWindow.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;

    class SettingsMenuView: public Pht::GuiView {
    public:
        enum class SceneId {
            Map,
            Game
        };

        SettingsMenuView(Pht::IEngine& engine,
                         const CommonResources& commonResources,
                         PotentiallyZoomedScreen potentiallyZoomedScreen);

        void EnableControlsSection();
        void DisableControlsSection();
        void SetMusicIsOn(bool musicIsOn);
        void SetSoundIsOn(bool soundIsOn);
        void SetGhostPieceIsEnabled(bool ghostPieceIsEnabled);
        void DeselectAllControlButtons();
        void DeselectAllClearEffectButtons();

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        bool IsControlsSectionEnabled() const {
            return mIsControlsSectionEnabled;
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
        
        RadioButton& GetDragAndDropButton() {
            return *mDragAndDropButton;
        }

        RadioButton& GetSwipeButton() {
            return *mSwipeButton;
        }

        RadioButton& GetSingleTapButton() {
            return *mSingleTapButton;
        }
        
        RadioButton& GetThrowButton() {
            return *mThrowButton;
        }

        RadioButton& GetShrinkButton() {
            return *mShrinkButton;
        }

    private:
        void CreateNineSmallBlocks(Pht::IEngine& engine,
                                   Pht::SceneObject& parent,
                                   const Pht::Vec3& position);
        void CreateThreeSmallBlocks(Pht::IEngine& engine,
                                    Pht::SceneObject& parent,
                                    const Pht::Vec3& position);
        void CreateLPieceIcon(Pht::IEngine& engine,
                              Pht::SceneObject& parent,
                              const Pht::Vec3& position,
                              const Pht::Vec2& blockSize,
                              float blockOffset,
                              float angle);
        Pht::SceneObject& CreateLPieceIcon(Pht::IEngine& engine,
                                           MenuButton& button,
                                           const Pht::Vec3& position,
                                           const Pht::Vec2& blockSize,
                                           float blockOffset,
                                           const Pht::Vec4& color,
                                           const Pht::Vec4& shadowColor,
                                           const Pht::Vec3& shadowOffset,
                                           float angle);
        void SetMusicOnIsVisible(bool isVisible);
        void SetMusicOffIsVisible(bool isVisible);
        void SetSoundOnIsVisible(bool isVisible);
        void SetSoundOffIsVisible(bool isVisible);
        void SetGhostPieceOnIsVisible(bool isVisible);
        void SetGhostPieceOffIsVisible(bool isVisible);

        const CommonResources& mCommonResources;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mGhostPieceButton;
        std::unique_ptr<MenuButton> mSoundButton;
        std::unique_ptr<MenuButton> mMusicButton;
        std::unique_ptr<RadioButton> mDragAndDropButton;
        std::unique_ptr<RadioButton> mSwipeButton;
        std::unique_ptr<RadioButton> mSingleTapButton;
        std::unique_ptr<RadioButton> mThrowButton;
        std::unique_ptr<RadioButton> mShrinkButton;
        Pht::SceneObject* mControlsSection {nullptr};
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
        bool mIsControlsSectionEnabled {true};
    };
}

#endif
