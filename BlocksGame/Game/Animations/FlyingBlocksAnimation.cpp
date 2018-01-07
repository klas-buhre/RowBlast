#include "FlyingBlocksAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

namespace {
    const auto subCellMass {1.0f};
    const auto rowExplosionForceMagnitude {12.0f};
    const auto explosiveForceMagnitude {15.0f};
    const Pht::Vec3 gravitationalAcceleration {0.0f, -60.0f, 0.0f};
    const auto eraseLimit {-27.0f};
}

FlyingBlocksAnimation::FlyingBlocksAnimation(const GameScene& scene) :
    mScene {scene} {}

void FlyingBlocksAnimation::AddBlockRows(const Field::RemovedSubCells& subCells) {
    for (auto& removedSubCell: subCells) {

        Pht::Vec3 explosiveForceDirecton {
            Pht::NormalizedRand() - 0.5f,
            Pht::NormalizedRand() - 0.5f,
            1.0f
        };
        explosiveForceDirecton.Normalize();
        
        auto force {explosiveForceDirecton * rowExplosionForceMagnitude};
    
        RigidBody rigidBody {
            .mPosition = CalculateBlockInitialPosition(removedSubCell),
            .mVelocity = force / subCellMass,
            .mOrientation.z = RotationToDeg(removedSubCell.mRotation),
            .mAngularVelocity = Pht::Vec3 {
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f
            },
            .mMass = subCellMass,
            .mRenderable = removedSubCell.mRenderable
        };

        mBodies.PushBack(rigidBody);
    }
}

void FlyingBlocksAnimation::AddBlocks(const Field::RemovedSubCells& subCells,
                                      const Pht::IVec2& detonationPos) {
    for (auto& removedSubCell: subCells) {
        auto dx {static_cast<float>(removedSubCell.mPosition.x - detonationPos.x)};
        auto dy {static_cast<float>(removedSubCell.mPosition.y - detonationPos.y)};
        
        Pht::Vec3 explosiveForceDirecton {dx, dy, 1.0f};
        explosiveForceDirecton.Normalize();
        
        auto forceMagnitude {0.0f};
        auto distSquare {dx * dx + dy * dy};
        if (distSquare == 0.0f) {
            forceMagnitude = explosiveForceMagnitude;
        } else {
            forceMagnitude = explosiveForceMagnitude / distSquare;
        }
        
        auto force {explosiveForceDirecton * forceMagnitude};
        
        auto angularVelocity {forceMagnitude * 50.0f};
    
        RigidBody rigidBody {
            .mPosition = CalculateBlockInitialPosition(removedSubCell),
            .mVelocity = force / subCellMass,
            .mOrientation.z = RotationToDeg(removedSubCell.mRotation),
            .mAngularVelocity = Pht::Vec3 {
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f,
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f,
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f
            },
            .mMass = subCellMass,
            .mRenderable = removedSubCell.mRenderable
        };

        mBodies.PushBack(rigidBody);
    }
}

void FlyingBlocksAnimation::Update(float dt) {
    auto i {0};

    while (i < mBodies.Size()) {
        auto& body {mBodies.At(i)};
        body.mVelocity += gravitationalAcceleration * dt;
        body.mPosition += body.mVelocity * dt;
        body.mOrientation += body.mAngularVelocity * dt;

        if (body.mPosition.y < eraseLimit) {
            mBodies.Erase(i);
        } else {
            ++i;
        }
    }
}

Pht::Vec3 FlyingBlocksAnimation::CalculateBlockInitialPosition(const RemovedSubCell& subCell) {
    auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};

    Pht::Vec3 position {
        static_cast<float>(subCell.mPosition.x) * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        static_cast<float>(subCell.mPosition.y) * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z
    };
    
    return position;
}
