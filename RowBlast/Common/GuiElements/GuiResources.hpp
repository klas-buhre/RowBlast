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
        const Pht::TextProperties& GetWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetCaptionTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
    
        const MenuWindow& GetLargeMenuWindowPotentiallyZoomedScreen() const {
            return mLargeMenuWindowPotentiallyZoomedScreen;
        }

        static const Pht::Color mBlueButtonColor;
        static const Pht::Color mBlueSelectedButtonColor;
        static const Pht::Color mYellowButtonColor;
        static const Pht::Color mYellowSelectedButtonColor;
        static const std::string mMediumButtonMeshFilename;

    private:
        MenuWindow mMediumMenuWindow;
        Pht::TextProperties mBlackButtonTextProperties;
        Pht::TextProperties mWhiteButtonTextProperties;
        Pht::TextProperties mCaptionTextProperties;
        Pht::TextProperties mSmallTextProperties;
        MenuWindow mLargeMenuWindowPotentiallyZoomedScreen;
        MenuWindow mMediumMenuWindowPotentiallyZoomedScreen;
        Pht::TextProperties mBlackButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mCaptionTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mSmallTextPropertiesPotentiallyZoomedScreen;
    };
}

#endif
