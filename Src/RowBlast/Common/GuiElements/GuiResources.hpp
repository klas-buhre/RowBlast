#ifndef GuiResources_hpp
#define GuiResources_hpp

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "MenuWindow.hpp"
#include "GlossyButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;

    class GuiResources {
    public:
        GuiResources(Pht::IEngine& engine, const CommonResources& commonResources);
        const MenuWindow& GetMediumDarkMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const;
        const MenuWindow& GetLargeDarkMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetBlackButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetLargeWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetCaptionTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetLargeWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;        
        Pht::RenderableObject& GetCloseButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetSmallBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetSmallDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;

        const MenuWindow& GetMediumMenuWindowPotentiallyZoomedScreen() const {
            return mMediumMenuWindowPotentiallyZoomedScreen;
        }
        
        const MenuWindow& GetLargeMenuWindowPotentiallyZoomedScreen() const {
            return mLargeMenuWindowPotentiallyZoomedScreen;
        }

        const MenuWindow& GetSmallDarkMenuWindowPotentiallyZoomedScreen() const {
            return mSmallDarkMenuWindowPotentiallyZoomedScreen;
        }
        
        const MenuWindow& GetSmallestDarkMenuWindowPotentiallyZoomedScreen() const {
            return mSmallestDarkMenuWindowPotentiallyZoomedScreen;
        }
        
        const Pht::TextProperties& GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen() const {
            return mWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen;
        }

        Pht::RenderableObject& GetSmallGrayGlossyButtonPotentiallyZoomedScreen() const {
            return *mSmallGrayGlossyButtonPotentiallyZoomedScreen;
        }

        Pht::RenderableObject& GetSmallerBlueGlossyButton() const {
            return *mSmallerBlueGlossyButton;
        }

        Pht::RenderableObject& GetSmallerDarkBlueGlossyButton() const {
            return *mSmallerDarkBlueGlossyButton;
        }

        Pht::RenderableObject& GetSmallestBlueGlossyButton() const {
            return *mSmallestBlueGlossyButton;
        }

        Pht::RenderableObject& GetSmallestDarkBlueGlossyButton() const {
            return *mSmallestDarkBlueGlossyButton;
        }

        static const Pht::Color mBlueButtonColor;
        static const Pht::Color mBlueSelectedButtonColor;
        static const Pht::Color mYellowButtonColor;
        static const Pht::Color mYellowSelectedButtonColor;

        static const std::string mMediumButtonSkewedMeshFilename;

    private:
        MenuWindow mMediumDarkMenuWindow;
        MenuWindow mLargeDarkMenuWindow;
        Pht::TextProperties mBlackButtonTextProperties;
        Pht::TextProperties mLargeWhiteButtonTextProperties;
        Pht::TextProperties mWhiteButtonTextProperties;
        Pht::TextProperties mCaptionTextProperties;
        Pht::TextProperties mSmallTextProperties;
        Pht::TextProperties mSmallWhiteTextProperties;
        Pht::TextProperties mLargeWhiteTextProperties;
        MenuWindow mLargeMenuWindowPotentiallyZoomedScreen;
        MenuWindow mMediumMenuWindowPotentiallyZoomedScreen;
        MenuWindow mLargeDarkMenuWindowPotentiallyZoomedScreen;
        MenuWindow mMediumDarkMenuWindowPotentiallyZoomedScreen;
        MenuWindow mSmallDarkMenuWindowPotentiallyZoomedScreen;
        MenuWindow mSmallestDarkMenuWindowPotentiallyZoomedScreen;
        Pht::TextProperties mBlackButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mLargeWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mCaptionTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mSmallTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mSmallWhiteTextPropertiesPotentiallyZoomedScreen;
        Pht::TextProperties mLargeWhiteTextPropertiesPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mCloseButton;
        std::unique_ptr<Pht::RenderableObject> mCloseButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mLargeBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargeBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mLargeDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargeDarkBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mMediumBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mMediumBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mMediumDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mMediumDarkBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mSmallBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mSmallDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallDarkBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mSmallGrayGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mLargeGreenGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargeGreenGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mLargeDarkGreenGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargeDarkGreenGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mSmallerBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallerDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallestBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallestDarkBlueGlossyButton;
    };
}

#endif
