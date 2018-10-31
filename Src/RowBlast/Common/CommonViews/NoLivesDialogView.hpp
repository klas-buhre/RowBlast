#ifndef NoLivesDialogView_hpp
#define NoLivesDialogView_hpp

#include <memory>
#include <chrono>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserData;
    
    class NoLivesDialogView: public Pht::GuiView {
    public:
        NoLivesDialogView(Pht::IEngine& engine,
                          const CommonResources& commonResources,
                          const UserData& userData,
                          PotentiallyZoomedScreen potentiallyZoomedScreen);

        void Update();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetRefillLivesButton() const {
            return *mRefillLivesButton;
        }
        
    private:
        const UserData& mUserData;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mRefillLivesButton;
        std::chrono::seconds mSecondsUntilNewLife;
        Pht::TextComponent* mCountdownText {nullptr};
    };
}

#endif