#ifndef PurchaseSuccessfulDialogView_hpp
#define PurchaseSuccessfulDialogView_hpp


#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class TextComponent;
}

namespace RowBlast {
    class PurchaseSuccessfulDialogView: public Pht::GuiView {
    public:
        PurchaseSuccessfulDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     PotentiallyZoomedScreen potentiallyZoomedScreen);

        void SetUp(int numCoins);
        void Update();

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetOkButton() const {
            return *mOkButton;
        }
        
    private:
        void CreateGlowEffect(const Pht::Vec3& position,
                              Pht::SceneObject& parentObject,
                              float scale);

        Pht::IEngine& mEngine;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mOkButton;
        Pht::TextComponent* mConfirmationText {nullptr};
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
    };
}

#endif
