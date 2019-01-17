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
        const Pht::TextProperties& GetBlackButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetLargeWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetCaptionTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetLargeWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::TextProperties& GetSmallWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const;        
        Pht::RenderableObject& GetCloseButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargerBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargerDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetSmallBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetSmallDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetLargeDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetMediumDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetThinBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;
        Pht::RenderableObject& GetThinDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const;

        const MenuWindow& GetMediumMenuWindow() const {
            return mMediumMenuWindow;
        }
        
        const MenuWindow& GetLargeMenuWindow() const {
            return mLargeMenuWindow;
        }

        const MenuWindow& GetSmallDarkMenuWindow() const {
            return mSmallDarkMenuWindow;
        }
        
        const MenuWindow& GetSmallestDarkMenuWindow() const {
            return mSmallestDarkMenuWindow;
        }

        const MenuWindow& GetMediumDarkMenuWindow() const {
            return mMediumDarkMenuWindow;
        }
    
        const MenuWindow& GetLargeDarkMenuWindow() const {
            return mLargeDarkMenuWindow;
        }

        const Pht::TextProperties& GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen() const {
            return mWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen;
        }

        Pht::RenderableObject& GetSmallGrayGlossyButtonPotentiallyZoomedScreen() const {
            return *mSmallGrayGlossyButtonPotentiallyZoomedScreen;
        }
        
        Pht::RenderableObject& GetMediumThinBlueGlossyButton() const {
            return *mMediumThinBlueGlossyButton;
        }

        Pht::RenderableObject& GetMediumThinDarkBlueGlossyButton() const {
            return *mMediumThinDarkBlueGlossyButton;
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
        Pht::TextProperties mBlackButtonTextProperties;
        Pht::TextProperties mLargeWhiteButtonTextProperties;
        Pht::TextProperties mWhiteButtonTextProperties;
        Pht::TextProperties mCaptionTextProperties;
        Pht::TextProperties mSmallTextProperties;
        Pht::TextProperties mSmallWhiteTextProperties;
        Pht::TextProperties mLargeWhiteTextProperties;
        MenuWindow mLargeMenuWindow;
        MenuWindow mMediumMenuWindow;
        MenuWindow mLargeDarkMenuWindow;
        MenuWindow mMediumDarkMenuWindow;
        MenuWindow mSmallDarkMenuWindow;
        MenuWindow mSmallestDarkMenuWindow;
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
        std::unique_ptr<Pht::RenderableObject> mLargerBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargerBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mLargerDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mLargerDarkBlueGlossyButtonPotentiallyZoomedScreen;
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
        std::unique_ptr<Pht::RenderableObject> mMediumGreenGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mMediumGreenGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mMediumDarkGreenGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mMediumDarkGreenGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mMediumThinBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mMediumThinDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallerBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallerDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallestBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mSmallestDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mThinBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mThinBlueGlossyButtonPotentiallyZoomedScreen;
        std::unique_ptr<Pht::RenderableObject> mThinDarkBlueGlossyButton;
        std::unique_ptr<Pht::RenderableObject> mThinDarkBlueGlossyButtonPotentiallyZoomedScreen;
    };
}

#endif
