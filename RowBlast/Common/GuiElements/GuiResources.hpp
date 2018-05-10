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
    
        const MenuWindow& GetLargeMenuWindow() const {
            return mLargeMenuWindow;
        }

        const MenuWindow& GetMediumMenuWindow() const {
            return mMediumMenuWindow;
        }
        
        const Pht::TextProperties& GetBlackButtonTextProperties() const {
            return mBlackButtonTextProperties;
        }

        const Pht::TextProperties& GetWhiteButtonTextProperties() const {
            return mWhiteButtonTextProperties;
        }

        const Pht::TextProperties& GetCaptionTextProperties() const {
            return mCaptionTextProperties;
        }

        const Pht::TextProperties& GetSmallTextProperties() const {
            return mSmallTextProperties;
        }

        static const Pht::Color mBlueButtonColor;
        static const Pht::Color mBlueSelectedButtonColor;
        static const Pht::Color mYellowButtonColor;
        static const Pht::Color mYellowSelectedButtonColor;
        static const std::string mMediumButtonMeshFilename;

    private:
        MenuWindow mLargeMenuWindow;
        MenuWindow mMediumMenuWindow;
        Pht::TextProperties mBlackButtonTextProperties;
        Pht::TextProperties mWhiteButtonTextProperties;
        Pht::TextProperties mCaptionTextProperties;
        Pht::TextProperties mSmallTextProperties;
    };
}

#endif
