#ifndef FloatingBlocks_hpp
#define FloatingBlocks_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace BlocksGame {
    class CommonResources;

    struct BlockPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
        bool mLPiece {false};
    };
    
    class FloatingBlocks {
    public:
        FloatingBlocks(Pht::IEngine& engine,
                       Pht::Scene& scene,
                       int layerIndex,
                       const std::vector<BlockPathVolume>& volumes,
                       const CommonResources& commonResources,
                       float scale);
        
        void Update();
        
    private:
        void InitCubes();
        
        static constexpr int numRenderables {4};

        struct FloatingBlock {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            std::unique_ptr<Pht::SceneObject> mSceneObject;
        };

        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        std::vector<FloatingBlock> mBlocks;
        std::array<std::unique_ptr<Pht::RenderableObject>, numRenderables> mBlockRenderables;
        std::vector<BlockPathVolume> mVolumes;
    };
}

#endif
