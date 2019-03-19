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
    class IGuiLightProvider;
    
    class GameMenuView: public Pht::GuiView {
    public:
        GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp();
        void EnableUndoButton();
        void DisableUndoButton();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }
        
        bool IsUndoButtonEnabled() const {
            return mIsUndoButtonEnabled;
        }
        
        const MenuButton& GetResumeButton() const {
            return *mResumeButton;
        }

        const MenuButton& GetUndoButton() const {
            return *mUndoButton;
        }
        
        const MenuButton& GetLevelInfoButton() const {
            return *mLevelInfoButton;
        }

        const MenuButton& GetHowToPlayButton() const {
            return *mHowToPlayButton;
        }

        const MenuButton& GetSettingsButton() const {
            return *mSettingsButton;
        }

        const MenuButton& GetRestartButton() const {
            return *mRestartButton;
        }

        const MenuButton& GetMapButton() const {
            return *mMapButton;
        }
        
    private:
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mResumeButton;
        std::unique_ptr<MenuButton> mUndoButton;
        std::unique_ptr<MenuButton> mLevelInfoButton;
        std::unique_ptr<MenuButton> mHowToPlayButton;
        std::unique_ptr<MenuButton> mSettingsButton;
        std::unique_ptr<MenuButton> mRestartButton;
        std::unique_ptr<MenuButton> mMapButton;
        bool mIsUndoButtonEnabled {true};
    };
}

#endif
