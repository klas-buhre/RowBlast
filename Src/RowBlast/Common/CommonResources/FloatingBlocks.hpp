#ifndef FloatingBlocks_hpp
#define FloatingBlocks_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"
#include "Optional.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class ISceneManager;
}

namespace RowBlast {
    class CommonResources;
    
    enum class FloatingPieceType {
        BigSingleBlock,
        SingleBlock,
        L,
        I,
        ShortI,
        B,
        Bomb,
        RowBomb,
        Asteroid,
        BigAsteroid
    };
    
    enum class FloatingBlockColor {
        Red = 0,
        Green,
        Blue,
        Gold,
        Gray,
        Random,
        RandomExceptGray,
        RandomOneOfEachColorExceptGray
    };

    struct BlockPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize {0.0f, 0.0f, 0.0f};
        FloatingPieceType mPieceType {FloatingPieceType::BigSingleBlock};
        FloatingBlockColor mBlockColor {FloatingBlockColor::RandomExceptGray};
        Pht::Optional<Pht::Vec3> mBlockRotation;
        Pht::Optional<int> mRotationAmplitude;
    };
    
    class FloatingBlocks {
    public:
        FloatingBlocks(Pht::IEngine& engine,
                       Pht::Scene& scene,
                       int layerIndex,
                       const std::vector<BlockPathVolume>& volumes,
                       const CommonResources& commonResources,
                       float scale,
                       float angularVelocity,
                       float staticRotationDuration);
        
        void Update();
        
    private:
        struct FloatingBlock {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            Pht::SceneObject* mSceneObject;
            float mElapsedTime {0.0f};
            float mRotationAmplitude {0.0f};
        };

        void CreateBomb(Pht::ISceneManager& sceneManager, float scale);
        void CreateRowBomb(Pht::ISceneManager& sceneManager, float scale);
        void CreateAsteroid(Pht::ISceneManager& sceneManager, float scale);
        void CreateBigAsteroid(Pht::ISceneManager& sceneManager, float scale);
        void InitBlocks(Pht::Scene& scene, float scale, float angularVelocity);
        Pht::RenderableObject& CalcBlockRenderable(const BlockPathVolume& volume,
                                                   std::vector<FloatingBlockColor>& colors);
        void CreateLPiece(Pht::SceneObject& pieceSceneObject,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
        void CreateIPiece(Pht::SceneObject& pieceSceneObject,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
        void CreateShortIPiece(Pht::SceneObject& pieceSceneObject,
                               float scale,
                               Pht::RenderableObject& renderable,
                               Pht::Scene& scene);
        void CreateBPiece(Pht::SceneObject& pieceSceneObject,
                          float scale,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene);
        void AnimateEmissive(float dt);
    
        static constexpr int numBlockRenderables {5};

        Pht::IEngine& mEngine;
        std::vector<FloatingBlock> mBlocks;
        std::array<std::unique_ptr<Pht::RenderableObject>, numBlockRenderables> mBlockRenderables;
        std::unique_ptr<Pht::RenderableObject> mBombRenderable;
        std::unique_ptr<Pht::RenderableObject> mRowBombRenderable;
        std::unique_ptr<Pht::RenderableObject> mAsteroidRenderable;
        std::unique_ptr<Pht::RenderableObject> mBigAsteroidRenderable;
        float mEmissiveAnimationTime {0.0f};
        float mRotationDuration {0.0f};
        std::vector<BlockPathVolume> mVolumes;
    };
}

#endif
