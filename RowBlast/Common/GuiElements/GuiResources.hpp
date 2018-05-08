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
    private:
        MenuWindow mLargeMenuWindow;
    };
}

#endif
