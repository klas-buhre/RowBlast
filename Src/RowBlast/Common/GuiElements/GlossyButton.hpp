#ifndef GlossyButton_hpp
#define GlossyButton_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "MenuWindow.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    enum class ButtonSize {
        Larger,
        Large,
        Medium,
        Small,
        Smaller,
        Smallest,
        Thin
    };

    enum class ButtonColor {
        Blue,
        Green,
        Gray,
        DarkBlue,
        DarkGreen
    };
    
    std::unique_ptr<Pht::RenderableObject>
    CreateGlossyButton(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       ButtonSize size,
                       ButtonColor color,
                       PotentiallyZoomedScreen potentiallyZoomed);
}

#endif
