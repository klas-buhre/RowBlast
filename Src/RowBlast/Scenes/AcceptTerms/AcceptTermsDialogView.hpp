#ifndef AcceptTermsDialogView_hpp
#define AcceptTermsDialogView_hpp

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

    class AcceptTermsDialogView: public Pht::GuiView {
    public:
        AcceptTermsDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        const MenuButton& GetTermsOfServiceButton() const {
            return *mTermsOfServiceButton;
        }

        const MenuButton& GetPrivacyPolicyButton() const {
            return *mPrivacyPolicyButton;
        }

        const MenuButton& GetAgreeButton() const {
            return *mAgreeButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mTermsOfServiceButton;
        std::unique_ptr<MenuButton> mPrivacyPolicyButton;
        std::unique_ptr<MenuButton> mAgreeButton;
    };
}

#endif
