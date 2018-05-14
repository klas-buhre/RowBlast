#ifndef GuiResources_hpp
#define GuiResources_hpp

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "MenuWindow.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;

    class GuiResources {
    public:
        GuiResources(Pht::IEngine& engine, const CommonResources& commonResources);
        const MenuWindow& GetMediumMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetBlackButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetLargeWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetCaptionTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetCloseButton(PotentiallyZoomedScreen potentiallyZoomed) const;
    
        const MenuWindow& GetLargeMenuWindowPotentiallyZoomedScreen() const {
            return mLargeMenuWindowPotentiallyZoomedScreen;
        }

        const MenuWindow& GetSmallMenuWindowPotentiallyZoomedScreen() const {
            return mSmallMenuWindowPotentiallyZoomedScreen;
        }

        static const Pht::Color mBlueButtonColor;
        static const Pht::Color mBlueSelectedButtonColor;
        static const Pht::Color mYellowButtonColor;
        static const Pht::Color mYellowSelectedButtonColor;
        static const Pht::Color mGreenButtonColor;
        static const Pht::Color mGreenSelectedButtonColor;

        static const std::string mBigButtonMeshFilename;
        static const std::string mMediumButtonMeshFilename;
        static const std::string mSmallButtonMeshFilename;

    private:
        MenuWindow mMediumMenuWindow;
        Pht::TextProperties mBlackButtonTextProperties;
        Pht::TextProperties mLargeWhiteButtonTextProperties;
        Pht::TextProperties mWhiteButtonTextProperties;
        Pht::TextProperties mCaptionTextProperties;
        Pht::TextProperties mSmallTextProperties;
        MenuWindow mLargeMenuWindowPotentiallyZoomedScreen;
        MenuWindow mMediumMenuWindowPotentiallyZoomedScreen;
        MenuWindow mSmallMenuWindowPotentiallyZoomedScreen;
        Pht::TextProperties mBlackButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mLargeWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mCaptionTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mSmallTextPropertiesPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mCloseButton;
        std::unique_ptr<Pht::RenderableObject> mCloseButtonPotentiallyZoomedScreen;
    };
}

#endif
