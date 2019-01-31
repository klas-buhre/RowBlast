#ifndef GameHudArrow_hpp
#define GameHudArrow_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class GameHudArrow {
    public:
        GameHudArrow(Pht::IEngine& engine, const CommonResources& commonResources);

        Pht::RenderableObject& GetArrowRenderable() const {
            return *mArrowRenderable;
        }

    private:
        std::unique_ptr<Pht::RenderableObject> mArrowRenderable;
    };
}

#endif
