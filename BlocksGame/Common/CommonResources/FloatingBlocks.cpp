#include "FloatingBlocks.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace BlocksGame;

FloatingBlocks::FloatingBlocks(Pht::IEngine& engine,
                               Pht::Scene& scene,
                               int layerIndex,
                               const std::vector<BlockPathVolume>& volumes,
                               const CommonResources& commonResources,
                               float scale) :
    mEngine {engine},
    mVolumes {volumes} {
    
    mBlocks.resize(mVolumes.size());
    
    Pht::ObjMesh cubeMesh {"cube_554.obj", scale};
    auto& materials {commonResources.GetMaterials()};
    auto& sceneManager {engine.GetSceneManager()};
    
    mBlockRenderables = {
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGoldMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetBlueMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetRedMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGreenMaterial())
    };
    
    mSceneObject = std::make_unique<Pht::SceneObject>();
    mSceneObject->SetLayer(layerIndex);
    
    scene.GetRoot().AddChild(*mSceneObject);
    
    for (auto& block: mBlocks) {
        block.mSceneObject = std::make_unique<Pht::SceneObject>();
        mSceneObject->AddChild(*block.mSceneObject);
    }
    
    InitCubes();
}

void FloatingBlocks::InitCubes() {
    for (auto i {0}; i < mBlocks.size(); ++i) {
        const auto& volume {mVolumes[i]};
    
        Pht::Vec3 position {
            Pht::NormalizedRand() * volume.mSize.x - volume.mSize.x / 2.0f + volume.mPosition.x,
            Pht::NormalizedRand() * volume.mSize.y - volume.mSize.y / 2.0f + volume.mPosition.y,
            Pht::NormalizedRand() * volume.mSize.z - volume.mSize.z / 2.0f + volume.mPosition.z
        };
        
        Pht::Vec3 velocity {
            volume.mSize == Pht::Vec3{0.0f, 0.0f, 0.0f} ? 0.0f : (Pht::NormalizedRand() - 0.5f),
            0.0f,
            0.0f
        };
        
        Pht::Vec3 rotation {
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
        };
        
        Pht::Vec3 angularVelocity {
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f
        };
        
        auto& block {mBlocks[i]};
        block.mVelocity = velocity;
        block.mAngularVelocity = angularVelocity;
        
        block.mSceneObject->SetRenderable(mBlockRenderables[std::rand() % numRenderables].get());
        
        auto& transform {block.mSceneObject->GetTransform()};
        transform.SetPosition(position);
        transform.SetRotation(rotation);
    }
}

void FloatingBlocks::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};

    for (auto i {0}; i < mBlocks.size(); ++i) {
        auto& block {mBlocks[i]};
        auto& transform {block.mSceneObject->GetTransform()};
        transform.Translate(block.mVelocity * dt);
        transform.Rotate(block.mAngularVelocity * dt);
        
        const auto& volume {mVolumes[i]};
        auto rightLimit {volume.mPosition.x + volume.mSize.x / 2.0f};
        auto leftLimit {volume.mPosition.x - volume.mSize.x / 2.0f};
        auto position {block.mSceneObject->GetWorldSpacePosition()};
        
        if (position.x > rightLimit && block.mVelocity.x > 0.0f) {
            block.mVelocity.x = -block.mVelocity.x;
        }
        
        if (position.x < leftLimit && block.mVelocity.x < 0.0f) {
            block.mVelocity.x = -block.mVelocity.x;
        }
    }
}
