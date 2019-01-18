#ifndef DocumentViewerDialogView_hpp
#define DocumentViewerDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;

    class DocumentViewerDialogView: public Pht::GuiView {
    public:
        DocumentViewerDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        void SetTermsOfServiceCaption();
        void SetPrivacyPolicyCaption();
        void SetCreditsCaption();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }

        const MenuButton& GetBackButton() const {
            return *mBackButton;
        }

    private:
        void HideAllCaptions();
        
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::SceneObject* mTermsOfServiceCaption {nullptr};
        Pht::SceneObject* mPrivacyPolicyCaption {nullptr};
        Pht::SceneObject* mCreditsCaption {nullptr};
    };
}
        
#endif
