#ifndef CloseButton_hpp
#define CloseButton_hpp

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
    
    std::unique_ptr<Pht::RenderableObject>
    CreateCloseButton(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      PotentiallyZoomedScreen potentiallyZoomed);
}

#endif
