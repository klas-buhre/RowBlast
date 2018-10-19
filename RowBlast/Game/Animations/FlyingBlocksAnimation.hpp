#ifndef FlyingBlocksAnimation_hpp
#define FlyingBlocksAnimation_hpp

#include <vector>
#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class LevelResources;
    class PieceResources;
    class BombsAnimation;

    class FlyingBlocksAnimation {
    public:
        struct FlyingBlock {
            enum class AppliedForce {
                ClearedLine,
                Explosion,
                RowExplosion
            };
            
            AppliedForce mAppliedForce {AppliedForce::ClearedLine};
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            Pht::SceneObject* mSceneObject {nullptr};
        };

        FlyingBlocksAnimation(GameScene& scene,
                              const LevelResources& levelResources,
                              const PieceResources& pieceResources,
                              const BombsAnimation& bombsAnimation);
        
        void Init();
        void Update(float dt);
        void AddBlockRows(const Field::RemovedSubCells& subCells);
        void AddBlocksRemovedByExplosion(const Field::RemovedSubCells& subCells,
                                         const Pht::IVec2& detonationPos,
                                         float explosiveForceMagnitude,
                                         bool applyForceToAlreadyFlyingBlocks);
        void AddBlocksRemovedByTheShield(const Field::RemovedSubCells& subCells,
                                         int numFieldColumns);

    private:
        Pht::SceneObject& SetupBlockSceneObject(const RemovedSubCell& subCell);
        void ApplyForceToAlreadyFlyingBlocks(float explosiveForceMagnitude,
                                             const Pht::IVec2& detonationPos);
        Pht::Vec3 CalculateBlockInitialPosition(const RemovedSubCell& subCell);
        Pht::RenderableObject& GetBlockRenderableObject(const RemovedSubCell& subCell);
        Pht::SceneObject& AccuireSceneObject();
        void ReleaseSceneObject(Pht::SceneObject& sceneObject);
        void UpdateBlocks(float dt);
        void HandleCollisions(float dt);
        
        static constexpr int maxNumBlockSceneObjects {Field::maxNumRows * Field::maxNumColumns};
        
        using FlyingBlocks = Pht::StaticVector<FlyingBlock, maxNumBlockSceneObjects>;
        using FreeSceneObjects = Pht::StaticVector<Pht::SceneObject*, maxNumBlockSceneObjects>;
        
        GameScene& mScene;
        const LevelResources& mLevelResources;
        const PieceResources& mPieceResources;
        const BombsAnimation& mBombsAnimation;
        FlyingBlocks mFlyingBlocks;
        std::vector<std::unique_ptr<Pht::SceneObject>> mSceneObjects;
        FreeSceneObjects mFreeSceneObjects;
        float mIntersectionDistanceSquared {0.0f};
    };
}

#endif
