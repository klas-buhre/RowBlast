#ifndef FlyingBlocksSystem_hpp
#define FlyingBlocksSystem_hpp

#include <vector>
#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "Field.hpp"

namespace RowBlast {
    class GameScene;
    class LevelResources;
    class PieceResources;
    class BombsAnimation;

    class FlyingBlocksSystem {
    public:
        struct FlyingBlock {
            enum class Effect {
                Fly,
                Shrink
            };
            
            enum class AppliedForce {
                ClearedLine,
                Explosion,
                RowExplosion
            };
            
            Effect mEffect {Effect::Fly};
            AppliedForce mAppliedForce {AppliedForce::ClearedLine};
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            Pht::SceneObject* mSceneObject {nullptr};
            float mScale {1.0f};
        };

        FlyingBlocksSystem(GameScene& scene,
                           const LevelResources& levelResources,
                           const PieceResources& pieceResources,
                           const BombsAnimation& bombsAnimation);
        
        void Init();
        void Update(float dt);
        void AddBlocks(const Field::RemovedSubCells& subCells);
        void AddBlockRowsWithShrinkEffect(const Field::RemovedSubCells& subCells);
        void AddBlocksRemovedByExplosion(const Field::RemovedSubCells& subCells,
                                         const Pht::IVec2& detonationPos,
                                         float explosiveForceMagnitude,
                                         bool applyForceToAlreadyFlyingBlocks);
        void AddBlocksRemovedByTheShield(const Field::RemovedSubCells& subCells,
                                         int numFieldColumns);

    private:
        Pht::SceneObject& SetUpBlockSceneObject(const RemovedSubCell& subCell);
        void ApplyForceToAlreadyFlyingBlocks(float explosiveForceMagnitude,
                                             const Pht::IVec2& detonationPos);
        Pht::Vec3 CalculateBlockInitialPosition(const RemovedSubCell& subCell);
        Pht::RenderableObject& GetBlockRenderableObject(const RemovedSubCell& subCell);
        Pht::SceneObject& AccuireSceneObject();
        void ReleaseSceneObject(Pht::SceneObject& sceneObject);
        void UpdateBlocks(float dt);
        bool UpdateFlyingBlock(FlyingBlock& flyingBlock, float dt);
        bool UpdateShrinkingBlock(FlyingBlock& flyingBlock, float dt);
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
