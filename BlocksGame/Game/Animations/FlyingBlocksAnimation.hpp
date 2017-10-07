#ifndef FlyingBlocksAnimation_hpp
#define FlyingBlocksAnimation_hpp

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
    class RenderableObject;
}

namespace BlocksGame {
    class GameScene;

    class FlyingBlocksAnimation {
    public:
        struct RigidBody {
            Pht::Vec3 mPosition;
            Pht::Vec3 mVelocity;
            Pht::Vec3 mOrientation;
            Pht::Vec3 mAngularVelocity;
            float mMass {0.0f};
            const Pht::RenderableObject* mRenderable {nullptr};
        };

        using RigidBodies = Pht::StaticVector<RigidBody, Field::maxNumRows * Field::maxNumColumns>;

        explicit FlyingBlocksAnimation(const GameScene& scene);
        
        void Update(float dt);
        void AddBlockRows(const Field::RemovedSubCells& subCells);
        void AddBlocks(const Field::RemovedSubCells& subCells, const Pht::IVec2& detonationPos);

        const RigidBodies& GetRigidBodies() const {
            return mBodies;
        }
            
    private:
        Pht::Vec3 CalculateBlockInitialPosition(const RemovedSubCell& subCell);
        
        const GameScene& mScene;
        RigidBodies mBodies;
    };
}

#endif

