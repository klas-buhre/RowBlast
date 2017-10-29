#ifndef NoLivesDialogView_hpp
#define NoLivesDialogView_hpp

#include <memory>
#include <chrono>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CloseButton.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    
    class NoLivesDialogView: public Pht::GuiView {
    public:
        NoLivesDialogView(Pht::IEngine& engine,
                          const CommonResources& commonResources,
                          const UserData& userData);

        void Update();
        
        const CloseButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetRefillLivesButton() const {
            return *mRefillLivesButton;
        }
        
    private:
        const UserData& mUserData;
        std::unique_ptr<CloseButton> mCloseButton;
        std::unique_ptr<MenuButton> mRefillLivesButton;
        std::chrono::seconds mSecondsUntilNewLife;
        Pht::Text* mCountdownText {nullptr};
    };
}

#endif
