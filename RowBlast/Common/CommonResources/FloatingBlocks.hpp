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
    
    enum class FloatingPieceType {
        BigSingleBlock,
        SingleBlock,
        L,
        I,
        ShortI,
        B
    };

    struct BlockPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize {0.0f, 0.0f, 0.0f};
        FloatingPieceType mPieceType {FloatingPieceType::BigSingleBlock};
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
        struct FloatingBlock {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            Pht::SceneObject* mSceneObject;
        };

        void InitBlocks(Pht::Scene& scene, float scale, float angularVelocity);
        void CreateLPiece(FloatingBlock& block,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
        void CreateIPiece(FloatingBlock& block,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
        void CreateShortIPiece(FloatingBlock& block,
                               float scale,
                               Pht::RenderableObject& renderable,
                               Pht::Scene& scene);
        void CreateBPiece(FloatingBlock& block,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
    
        static constexpr int numRenderables {4};

        Pht::IEngine& mEngine;
        std::vector<FloatingBlock> mBlocks;
        std::array<std::unique_ptr<Pht::RenderableObject>, numRenderables> mBlockRenderables;
        std::vector<BlockPathVolume> mVolumes;
    };
}

#endif
