#ifndef FloatingBlocks_hpp
#define FloatingBlocks_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;

    struct BlockPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
        bool mIsLPiece {false};
    };
    
    class FloatingBlocks {
    public:
        FloatingBlocks(Pht::IEngine& engine,
                       Pht::Scene& scene,
                       int layerIndex,
                       const std::vector<BlockPathVolume>& volumes,
                       const CommonResources& commonResources,
                       float scale,
                       float angularVelocity);
        
        void Update();
        
    private:
        void InitBlocks(Pht::Scene& scene, float scale, float angularVelocity);
        
        static constexpr int numRenderables {4};

        struct FloatingBlock {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            Pht::SceneObject* mSceneObject;
        };

        Pht::IEngine& mEngine;
        std::vector<FloatingBlock> mBlocks;
        std::array<std::unique_ptr<Pht::RenderableObject>, numRenderables> mBlockRenderables;
        std::vector<BlockPathVolume> mVolumes;
    };
}

#endif
