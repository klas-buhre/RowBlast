#include "FlyingBlocksAnimation.hpp"

// Engine includes.
#include "MathUtils.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "GameScene.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"
#include "BombsAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto subCellMass = 1.0f;
    constexpr auto rowExplosionForceMagnitude = 12.0f;
    constexpr auto shieldForceMagnitude = 13.0f;
    const Pht::Vec3 gravitationalAcceleration {0.0f, -65.0f, 0.0f};
    const Pht::Vec3 explosionGravitationalAcceleration {0.0f, -40.0f, 0.0f};
    constexpr auto eraseLimit = 20.0f;
}

FlyingBlocksSystem::FlyingBlocksSystem(GameScene& scene,
                                       const LevelResources& levelResources,
                                       const PieceResources& pieceResources,
                                       const BombsAnimation& bombsAnimation) :
    mScene {scene},
    mLevelResources {levelResources},
    mPieceResources {pieceResources},
    mBombsAnimation {bombsAnimation} {
    
    mSceneObjects.resize(maxNumBlockSceneObjects);
    
    for (auto& sceneObject: mSceneObjects) {
        sceneObject = std::make_unique<Pht::SceneObject>();
    }
    
    auto blockSize = scene.GetCellSize();
    mIntersectionDistanceSquared = blockSize * blockSize;
}

void FlyingBlocksSystem::Init() {
    mFlyingBlocks.Clear();
    mFreeSceneObjects.Clear();
    
    auto& containerObject = mScene.GetFlyingBlocksContainer();
    
    for (auto& sceneObject: mSceneObjects) {
        mFreeSceneObjects.PushBack(sceneObject.get());
        containerObject.AddChild(*sceneObject);
        sceneObject->SetIsVisible(false);
        sceneObject->SetIsStatic(true);
    }
}

void FlyingBlocksSystem::AddBlockRows(const Field::RemovedSubCells& subCells) {
    for (auto& removedSubCell: subCells) {
        Pht::Vec3 explosiveForceDirecton {
            Pht::NormalizedRand() - 0.5f,
            Pht::NormalizedRand() - 0.5f,
            1.0f
        };

        explosiveForceDirecton.Normalize();
        auto force = explosiveForceDirecton * rowExplosionForceMagnitude;

        FlyingBlock flyingBlock {
            .mVelocity = force / subCellMass,
            .mAngularVelocity = Pht::Vec3 {
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f
            },
            .mSceneObject = &SetUpBlockSceneObject(removedSubCell)
        };
        
        if (removedSubCell.mFlags.mIsAsteroidFragment) {
            flyingBlock.mAppliedForce = FlyingBlock::AppliedForce::Explosion;
        }

        mFlyingBlocks.PushBack(flyingBlock);
    }
}

Pht::SceneObject& FlyingBlocksSystem::SetUpBlockSceneObject(const RemovedSubCell& removedSubCell) {
    auto& sceneObject = AccuireSceneObject();
    sceneObject.SetRenderable(&GetBlockRenderableObject(removedSubCell));
    auto& transform = sceneObject.GetTransform();
    transform.SetPosition(CalculateBlockInitialPosition(removedSubCell));

    switch (removedSubCell.mBlockKind) {
        case BlockKind::Bomb:
            transform.SetRotation(mBombsAnimation.GetBombStaticRotation());
            break;
        case BlockKind::RowBomb:
            transform.SetRotation(mBombsAnimation.GetRowBombStaticRotation());
            break;
        default:
            transform.SetRotation({0.0f, 0.0f, RotationToDeg(removedSubCell.mRotation)});
            break;
    }

    return sceneObject;
}

void FlyingBlocksSystem::AddBlocksRemovedByExplosion(const Field::RemovedSubCells& subCells,
                                                     const Pht::IVec2& detonationPos,
                                                     float explosiveForceMagnitude,
                                                     bool applyForceToAlreadyFlyingBlocks) {
    if (applyForceToAlreadyFlyingBlocks) {
        ApplyForceToAlreadyFlyingBlocks(explosiveForceMagnitude, detonationPos);
    }

    for (auto& removedSubCell: subCells) {
        auto dx = removedSubCell.mExactPosition.x - static_cast<float>(detonationPos.x);
        auto dy = removedSubCell.mExactPosition.y - static_cast<float>(detonationPos.y);
        
        if (dx == 0.0f) {
            dx = dy * (Pht::NormalizedRand() * 0.2f - 0.1f);
        }
        
        if (dy == 0.0f) {
            dy = dx * (Pht::NormalizedRand() * 0.2f - 0.1f);
        }

        Pht::Vec3 explosiveForceDirection {dx, dy, 1.0f};
        explosiveForceDirection.Normalize();
        
        auto forceMagnitude = 0.0f;
        auto distSquare = dx * dx + dy * dy;
        if (distSquare == 0.0f) {
            forceMagnitude = explosiveForceMagnitude;
        } else {
            forceMagnitude = explosiveForceMagnitude / distSquare;
        }
        
        auto force = explosiveForceDirection * forceMagnitude;
        auto angularVelocity = forceMagnitude * 50.0f;
        
        auto& sceneObject = AccuireSceneObject();
        sceneObject.SetRenderable(&GetBlockRenderableObject(removedSubCell));
        auto& transform = sceneObject.GetTransform();
        transform.SetPosition(CalculateBlockInitialPosition(removedSubCell));
        transform.SetRotation({0.0f, 0.0f, RotationToDeg(removedSubCell.mRotation)});
    
        FlyingBlock flyingBlock {
            .mAppliedForce = FlyingBlock::AppliedForce::Explosion,
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

void FlyingBlocksSystem::ApplyForceToAlreadyFlyingBlocks(float explosiveForceMagnitude,
                                                         const Pht::IVec2& detonationPos) {
    auto cellSize = mScene.GetCellSize();
    auto& fieldLowerLeft = mScene.GetFieldLoweLeft();

    Pht::Vec3 detonationPosWorldSpace {
        static_cast<float>(detonationPos.x) * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        static_cast<float>(detonationPos.y) * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z
    };
    
    for (auto& block: mFlyingBlocks) {
        auto& blockPosition = block.mSceneObject->GetTransform().GetPosition();
        auto dx = (blockPosition.x - static_cast<float>(detonationPosWorldSpace.x)) / cellSize;
        auto dy = (blockPosition.y - static_cast<float>(detonationPosWorldSpace.y)) / cellSize;

        Pht::Vec3 explosiveForceDirecton {dx, dy, 1.0f};
        explosiveForceDirecton.Normalize();
        
        auto forceMagnitude = 0.0f;
        auto distSquare = dx * dx + dy * dy;
        if (distSquare == 0.0f) {
            forceMagnitude = explosiveForceMagnitude;
        } else {
            forceMagnitude = explosiveForceMagnitude / distSquare;
        }
        
        auto force = explosiveForceDirecton * forceMagnitude;
        block.mVelocity += force / subCellMass;
    }
}

void FlyingBlocksSystem::AddBlocksRemovedByTheShield(const Field::RemovedSubCells& subCells,
                                                     int numFieldColumns) {
    
    for (auto& removedSubCell: subCells) {
        Pht::Vec3 shieldForceDirecton {
            removedSubCell.mGridPosition.x <= numFieldColumns / 2 ?
                (-1.0f - Pht::NormalizedRand()) : (1.0f + Pht::NormalizedRand()),
            Pht::NormalizedRand() - 0.25f,
            1.0f
        };

        shieldForceDirecton.Normalize();
        auto force = shieldForceDirecton * shieldForceMagnitude;

        FlyingBlock flyingBlock {
            .mVelocity = force / subCellMass,
            .mAngularVelocity = Pht::Vec3 {
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f,
                720.0f * Pht::NormalizedRand() - 360.0f
            },
            .mSceneObject = &SetUpBlockSceneObject(removedSubCell)
        };

        mFlyingBlocks.PushBack(flyingBlock);
    }
}

void FlyingBlocksSystem::Update(float dt) {
    UpdateBlocks(dt);
    HandleCollisions(dt);
}

void FlyingBlocksSystem::UpdateBlocks(float dt) {
    auto i = 0;

    while (i < mFlyingBlocks.Size()) {
        auto& flyingBlock = mFlyingBlocks.At(i);
        switch (flyingBlock.mAppliedForce) {
            case FlyingBlock::AppliedForce::ClearedLine:
            case FlyingBlock::AppliedForce::RowExplosion:
                flyingBlock.mVelocity += gravitationalAcceleration * dt;
                break;
            case FlyingBlock::AppliedForce::Explosion:
                flyingBlock.mVelocity += explosionGravitationalAcceleration * dt;
                break;
        }
        
        auto& transform = flyingBlock.mSceneObject->GetTransform();
        transform.Translate(flyingBlock.mVelocity * dt);
        transform.Rotate(flyingBlock.mAngularVelocity * dt);
        
        auto position = transform.GetPosition();
        auto& cameraPosition = mScene.GetCamera().GetSceneObject().GetTransform().GetPosition();
        
        if (position.z > cameraPosition.z - 5.0f) {
            position.z = cameraPosition.z - 5.0f;
            transform.SetPosition(position);
        }

        if (transform.GetPosition().y < cameraPosition.y - eraseLimit) {
            ReleaseSceneObject(*flyingBlock.mSceneObject);
            mFlyingBlocks.Erase(i);
        } else {
            ++i;
        }
    }
}

void FlyingBlocksSystem::HandleCollisions(float dt) {
    auto numBlocks = mFlyingBlocks.Size();
    
    for (auto i = 0; i < numBlocks; ++i) {
        for (auto j = i + 1; j < numBlocks; ++j) {
            auto& block1 = mFlyingBlocks.At(i);
            auto& block2 = mFlyingBlocks.At(j);
            auto& block1Position = block1.mSceneObject->GetTransform().GetPosition();
            auto& block2Position = block2.mSceneObject->GetTransform().GetPosition();
            
            Pht::Vec3 pos1MinusPos2 {block1Position - block2Position};
            auto distSquared = pos1MinusPos2.LengthSquared();
            if (distSquared > mIntersectionDistanceSquared) {
                continue;
            }
            
            if (distSquared == 0.0f) {
                distSquared = 0.01f;
            }
            
            Pht::Vec3 pos2MinusPos1 {block2Position - block1Position};
            auto v1MinusV2 = block1.mVelocity - block2.mVelocity;
            auto v2MinusV1 = block2.mVelocity - block1.mVelocity;
            
            // Equations from wikipedia: https://en.wikipedia.org/wiki/Elastic_collision
            // section "Two-dimensional collision with two moving objects". Used in 3d space here.
            auto dv1 = -pos1MinusPos2 * v1MinusV2.Dot(pos1MinusPos2) / distSquared;
            auto dv2 = -pos2MinusPos1 * v2MinusV1.Dot(pos2MinusPos1) / distSquared;

            auto newPos1 = block1Position + (block1.mVelocity + dv1) * dt;
            auto newPos2 = block2Position + (block2.mVelocity + dv2) * dt;
            auto newDistSquared = (newPos1 - newPos2).LengthSquared();
            
            if (newDistSquared > distSquared) {
                block1.mVelocity += dv1;
                block2.mVelocity += dv2;
            }
        }
    }
}

Pht::Vec3 FlyingBlocksSystem::CalculateBlockInitialPosition(const RemovedSubCell& subCell) {
    auto cellSize = mScene.GetCellSize();
    auto& fieldLowerLeft = mScene.GetFieldLoweLeft();

    Pht::Vec3 position {
        subCell.mExactPosition.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        subCell.mExactPosition.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z
    };
    
    return position;
}

Pht::RenderableObject& FlyingBlocksSystem::GetBlockRenderableObject(const RemovedSubCell& subCell) {
    if (subCell.mFlags.mIsGrayLevelBlock) {
        return mLevelResources.GetLevelBlockRenderable(subCell.mBlockKind);
    } else if (subCell.mFlags.mIsAsteroidFragment) {
        return mLevelResources.GetAsteroidFragmentRenderable();
    } else if (subCell.mBlockKind == BlockKind::Bomb) {
        return mLevelResources.GetLevelBombRenderable();
    } else if (subCell.mBlockKind == BlockKind::RowBomb) {
        return mPieceResources.GetRowBombRenderableObject();
    } else {
        auto brightness =
            subCell.mFlashingBlockAnimationState == FlashingBlockAnimationComponent::State::Waiting ||
            subCell.mFlashingBlockAnimationState == FlashingBlockAnimationComponent::State::Active ?
            BlockBrightness::Flashing : BlockBrightness::Normal;
        return mPieceResources.GetBlockRenderableObject(subCell.mBlockKind,
                                                        subCell.mColor,
                                                        brightness);
    }
}

Pht::SceneObject& FlyingBlocksSystem::AccuireSceneObject() {
    assert(mFreeSceneObjects.Size() >= 1);
    auto* sceneObject = mFreeSceneObjects.Back();
    mFreeSceneObjects.PopBack();
    sceneObject->SetIsVisible(true);
    sceneObject->SetIsStatic(false);
    return *sceneObject;
}

void FlyingBlocksSystem::ReleaseSceneObject(Pht::SceneObject& sceneObject) {
    sceneObject.SetIsVisible(false);
    sceneObject.SetIsStatic(true);
    mFreeSceneObjects.PushBack(&sceneObject);
}
