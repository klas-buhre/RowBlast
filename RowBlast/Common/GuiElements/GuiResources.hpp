#ifndef GuiResources_hpp
#define GuiResources_hpp

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
        
        static const Pht::Color mBlueButtonColor;
        static const Pht::Color mBlueSelectedButtonColor;
        static const Pht::Color mYellowButtonColor;
        static const Pht::Color mYellowSelectedButtonColor;
        static const std::string mMediumButtonMeshFilename;

    private:
        MenuWindow mLargeMenuWindow;
    };
}

#endif
