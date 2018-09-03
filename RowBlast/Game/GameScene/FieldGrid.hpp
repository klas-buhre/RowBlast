#ifndef FieldGrid_hpp
#define FieldGrid_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;

    class FieldGrid {
    public:
        FieldGrid(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources);
    
        void Init(int numRows);
        
    private:
        GameScene& mScene;
        std::unique_ptr<Pht::RenderableObject> mGridSegmentRenderable;
    };
}

#endif
