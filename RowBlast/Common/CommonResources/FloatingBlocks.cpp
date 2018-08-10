#include "FloatingBlocks.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "SceneObject.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    void CreateBlock(const Pht::Vec3& position,
                     float scale,
                     Pht::RenderableObject& renderable,
                     Pht::Scene& scene,
                     Pht::SceneObject& parent) {
        auto& sceneObject {scene.CreateSceneObject()};
        sceneObject.SetRenderable(&renderable);
        
        auto& transform {sceneObject.GetTransform()};
        transform.SetScale(scale);
        transform.SetPosition(position);
        
        parent.AddChild(sceneObject);
    }
}

FloatingBlocks::FloatingBlocks(Pht::IEngine& engine,
                               Pht::Scene& scene,
                               int layerIndex,
                               const std::vector<BlockPathVolume>& volumes,
                               const CommonResources& commonResources,
                               float scale,
                               float angularVelocity) :
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
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGreenMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetLightGrayMaterial())
    };
    
    auto& sceneObject {scene.CreateSceneObject()};
    sceneObject.SetLayer(layerIndex);
    scene.GetRoot().AddChild(sceneObject);
    
    for (auto& block: mBlocks) {
        block.mSceneObject = &scene.CreateSceneObject();
        sceneObject.AddChild(*block.mSceneObject);
    }
    
    InitBlocks(scene, scale, angularVelocity);
}

void FloatingBlocks::InitBlocks(Pht::Scene& scene, float scale, float angularVelocity) {
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
        
        Pht::Vec3 blockAngularVelocity {
            (Pht::NormalizedRand() - 0.5f) * angularVelocity,
            (Pht::NormalizedRand() - 0.5f) * angularVelocity,
            (Pht::NormalizedRand() - 0.5f) * angularVelocity
        };
        
        auto& block {mBlocks[i]};
        block.mVelocity = velocity;
        block.mAngularVelocity = blockAngularVelocity;
        
        auto& renderable {CalcBlockRenderable(volume.mBlockColor)};
        
        switch (volume.mPieceType) {
            case FloatingPieceType::BigSingleBlock:
                block.mSceneObject->SetRenderable(&renderable);
                break;
            case FloatingPieceType::SingleBlock: {
                auto blockSize {scale / 1.7f};
                auto blockScale {blockSize / scale};
                CreateBlock({0.0f, 0.0f, 0.0f}, blockScale, renderable, scene, *block.mSceneObject);
                break;
            }
            case FloatingPieceType::L:
                CreateLPiece(block, scale, renderable, scene);
                break;
            case FloatingPieceType::I:
                CreateIPiece(block, scale, renderable, scene);
                break;
            case FloatingPieceType::ShortI:
                CreateShortIPiece(block, scale, renderable, scene);
                break;
            case FloatingPieceType::B:
                CreateBPiece(block, scale, renderable, scene);
                break;
        }
        
        auto& transform {block.mSceneObject->GetTransform()};
        transform.SetPosition(position);
        transform.SetRotation(rotation);
    }
}

Pht::RenderableObject& FloatingBlocks::CalcBlockRenderable(FloatingBlockColor color) {
    switch (color) {
        case FloatingBlockColor::Random:
            return *mBlockRenderables[std::rand() % numRenderables];
        case FloatingBlockColor::RandomExceptGray:
            return *mBlockRenderables[std::rand() % (numRenderables - 1)];
        case FloatingBlockColor::Gray:
            return *mBlockRenderables[numRenderables - 1];
    }
}

void FloatingBlocks::CreateLPiece(FloatingBlock& block,
                                  float scale,
                                  Pht::RenderableObject& renderable,
                                  Pht::Scene& scene) {
    auto blockSize {scale / 1.7f};
    auto blockScale {blockSize / scale};

    CreateBlock({-blockSize / 2.0f, blockSize / 2.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, blockSize / 2.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, -blockSize / 2.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
}

void FloatingBlocks::CreateIPiece(FloatingBlock& block,
                                  float scale,
                                  Pht::RenderableObject& renderable,
                                  Pht::Scene& scene) {
    auto blockSize {scale / 1.7f};
    auto blockScale {blockSize / scale};

    CreateBlock({-blockSize, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({0.0f, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
}

void FloatingBlocks::CreateShortIPiece(FloatingBlock& block,
                                       float scale,
                                       Pht::RenderableObject& renderable,
                                       Pht::Scene& scene) {
    auto blockSize {scale / 1.7f};
    auto blockScale {blockSize / scale};
    
    CreateBlock({-blockSize / 2.0f, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
}

void FloatingBlocks::CreateBPiece(FloatingBlock& block,
                                  float scale,
                                  Pht::RenderableObject& renderable,
                                  Pht::Scene& scene) {
    auto blockSize {scale / 1.7f};
    auto blockScale {blockSize / scale};

    CreateBlock({-blockSize / 2.0f, -blockSize, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({-blockSize / 2.0f, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, -blockSize, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, 0.0f, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
    CreateBlock({blockSize / 2.0f, blockSize, 0.0f},
                blockScale,
                renderable,
                scene,
                *block.mSceneObject);
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
