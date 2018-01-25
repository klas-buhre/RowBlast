#include "FlyingBlocksAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"

using namespace BlocksGame;

namespace {
    const auto subCellMass {1.0f};
    const auto rowExplosionForceMagnitude {12.0f};
    const auto explosiveForceMagnitude {15.0f};
    const Pht::Vec3 gravitationalAcceleration {0.0f, -60.0f, 0.0f};
    const auto eraseLimit {-27.0f};
}

FlyingBlocksAnimation::FlyingBlocksAnimation(GameScene& scene,
                                             const LevelResources& levelResources,
                                             const PieceResources& pieceResources) :
    mScene {scene},
    mLevelResources {levelResources},
    mPieceResources {pieceResources} {
    
    mSceneObjects.resize(maxNumBlockSceneObjects);
    
    for (auto& sceneObject: mSceneObjects) {
        sceneObject = std::make_unique<Pht::SceneObject>();
    }
}

void FlyingBlocksAnimation::Reset() {
    mFlyingBlocks.Clear();
    mFreeSceneObjects.Clear();
    
    auto& containerObject {mScene.GetFlyingBlocksContainer()};
    
    for (auto& sceneObject: mSceneObjects) {
        mFreeSceneObjects.PushBack(sceneObject.get());
        containerObject.AddChild(*sceneObject);
        sceneObject->SetIsVisible(false);
    }
}

void FlyingBlocksAnimation::AddBlockRows(const Field::RemovedSubCells& subCells) {
    for (auto& removedSubCell: subCells) {

        Pht::Vec3 explosiveForceDirecton {
            Pht::NormalizedRand() - 0.5f,
            Pht::NormalizedRand() - 0.5f,
            1.0f
        };
        explosiveForceDirecton.Normalize();
        auto force {explosiveForceDirecton * rowExplosionForceMagnitude};
        
        auto& sceneObject {AccuireSceneObject()};
        sceneObject.SetRenderable(&GetBlockRenderableObject(removedSubCell));
        auto& transform {sceneObject.GetTransform()};
        transform.SetPosition(CalculateBlockInitialPosition(removedSubCell));
        transform.SetRotation({0.0f, 0.0f, RotationToDeg(removedSubCell.mRotation)});
        
        FlyingBlock flyingBlock {
            .mVelocity = force / subCellMass,
            .mAngularVelocity = Pht::Vec3 {
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f
            },
            .mSceneObject = &sceneObject
        };

        mFlyingBlocks.PushBack(flyingBlock);
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
        
        auto& sceneObject {AccuireSceneObject()};
        sceneObject.SetRenderable(&GetBlockRenderableObject(removedSubCell));
        auto& transform {sceneObject.GetTransform()};
        transform.SetPosition(CalculateBlockInitialPosition(removedSubCell));
        transform.SetRotation({0.0f, 0.0f, RotationToDeg(removedSubCell.mRotation)});
    
        FlyingBlock flyingBlock {
            .mVelocity = force / subCellMass,
            .mAngularVelocity = Pht::Vec3 {
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f,
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f,
                angularVelocity * Pht::NormalizedRand() - angularVelocity / 2.0f
            },
            .mSceneObject = &sceneObject
        };

        mFlyingBlocks.PushBack(flyingBlock);
    }
}

void FlyingBlocksAnimation::Update(float dt) {
    auto i {0};

    while (i < mFlyingBlocks.Size()) {
        auto& flyingBlock {mFlyingBlocks.At(i)};
        flyingBlock.mVelocity += gravitationalAcceleration * dt;
        
        auto& transform {flyingBlock.mSceneObject->GetTransform()};
        transform.Translate(flyingBlock.mVelocity * dt);
        transform.Rotate(flyingBlock.mAngularVelocity * dt);

        if (transform.GetPosition().y < eraseLimit) {
            ReleaseSceneObject(*flyingBlock.mSceneObject);
            mFlyingBlocks.Erase(i);
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

Pht::RenderableObject& FlyingBlocksAnimation::GetBlockRenderableObject(const RemovedSubCell& subCell) {
    if (subCell.mIsLevel) {
        return mLevelResources.GetLevelBlockRenderable(subCell.mRenderableKind);
    }
    
    return mPieceResources.GetBlockRenderableObject(subCell.mRenderableKind,
                                                    subCell.mColor,
                                                    BlockBrightness::Normal);
}

Pht::SceneObject& FlyingBlocksAnimation::AccuireSceneObject() {
    assert(mFreeSceneObjects.Size() >= 1);
    auto* sceneObject {mFreeSceneObjects.Back()};
    mFreeSceneObjects.PopBack();
    sceneObject->SetIsVisible(true);
    return *sceneObject;
}

void FlyingBlocksAnimation::ReleaseSceneObject(Pht::SceneObject& sceneObject) {
    sceneObject.SetIsVisible(false);
    mFreeSceneObjects.PushBack(&sceneObject);
}
