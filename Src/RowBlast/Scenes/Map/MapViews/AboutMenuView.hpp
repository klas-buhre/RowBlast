#ifndef AboutMenuView_hpp
#define AboutMenuView_hpp

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
    
    class AboutMenuView: public Pht::GuiView {
    public:
        AboutMenuView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }

        const MenuButton& GetTermsOfServiceButton() const {
            return *mTermsOfServiceButton;
        }

        const MenuButton& GetPrivacyPolicyButton() const {
            return *mPrivacyPolicyButton;
        }

        const MenuButton& GetCreditsButton() const {
            return *mCreditsButton;
        }

        const MenuButton& GetBackButton() const {
            return *mBackButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mTermsOfServiceButton;
        std::unique_ptr<MenuButton> mPrivacyPolicyButton;
        std::unique_ptr<MenuButton> mCreditsButton;
        std::unique_ptr<MenuButton> mBackButton;
    };
}

#endif
