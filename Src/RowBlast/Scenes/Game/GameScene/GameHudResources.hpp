#ifndef GameHudResources_hpp
#define GameHudResources_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class GameHudResources {
    public:
        GameHudResources(Pht::IEngine& engine, const CommonResources& commonResources);

        Pht::RenderableObject& GetDownArrowRenderable() const {
            return *mDownArrowRenderable;
        }

        Pht::RenderableObject& GetArrowMeshRenderable() const {
            return *mArrowMeshRenderable;
        }

    private:
        void CreateDownArrow(Pht::IEngine& engine, const CommonResources& commonResources);
        void CreateArrowMesh(Pht::IEngine& engine, const CommonResources& commonResources);
        
        std::unique_ptr<Pht::RenderableObject> mDownArrowRenderable;
        std::unique_ptr<Pht::RenderableObject> mArrowMeshRenderable;
    };
}

#endif
