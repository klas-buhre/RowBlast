#include "FloatingBlocks.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "SceneObject.hpp"
#include "MathUtils.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto rotationDuration = 6.0f;
    constexpr auto rotationAmplitude = 5.5f;
    constexpr auto emissiveAnimationDuration = 1.5f;
    constexpr auto emissiveAmplitude = 1.7f;

    void CreateBlock(const Pht::Vec3& position,
                     float scale,
                     Pht::RenderableObject& renderable,
                     Pht::Scene& scene,
                     Pht::SceneObject& parent) {
        auto& sceneObject = scene.CreateSceneObject();
        sceneObject.SetRenderable(&renderable);
        
        auto& transform = sceneObject.GetTransform();
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
    auto& materials = commonResources.GetMaterials();
    auto& sceneManager = engine.GetSceneManager();
    
    mBlockRenderables = {
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetRedMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGreenMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetBlueMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGoldMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetLightGrayMaterial())
    };
    
    CreateBomb(sceneManager, scale);
    CreateRowBomb(sceneManager, scale);
    CreateAsteroid(sceneManager, scale);
    CreateBigAsteroid(sceneManager, scale);
    
    auto& sceneObject = scene.CreateSceneObject();
    sceneObject.SetLayer(layerIndex);
    scene.GetRoot().AddChild(sceneObject);
    
    for (auto& block: mBlocks) {
        block.mSceneObject = &scene.CreateSceneObject();
        sceneObject.AddChild(*block.mSceneObject);
    }
    
    InitBlocks(scene, scale, angularVelocity);
}

void FloatingBlocks::CreateBomb(Pht::ISceneManager& sceneManager, float scale) {
    Pht::Material bombMaterial {
        "bomb_798.jpg",
        "bomb_798_emission.jpg",
        1.0f,
        0.87f,
        1.0f,
        1.0f,
        30.0f
    };

    bombMaterial.SetEmissive(Pht::Color {3.0f, 3.0f, 3.0f});
    mBombRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 8.1f * scale},
                                                          bombMaterial);
}

void FloatingBlocks::CreateRowBomb(Pht::ISceneManager& sceneManager, float scale) {
    Pht::Material rowBombMaterial {
        "laser_bomb_diffuse.jpg",
        "laser_bomb_emission.png",
        0.55f,
        0.65f,
        0.05f,
        1.0f,
        20.0f
    };
    
    rowBombMaterial.SetAmbient(Pht::Color {0.5f, 0.5f, 0.85f});
    rowBombMaterial.SetEmissive(Pht::Color {2.0f, 2.0f, 2.0f});
    mRowBombRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"laser_bomb_224.obj", 0.3f * scale},
                                                             rowBombMaterial);
}

void FloatingBlocks::CreateAsteroid(Pht::ISceneManager& sceneManager, float scale) {
    Pht::Material asteroidMaterial {"brown_asteroid.jpg", 0.425f, 1.0f, 0.05f, 1.0f};
    mAsteroidRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"asteroid_998.obj", 17.6f * scale},
                                                              asteroidMaterial);
}

void FloatingBlocks::CreateBigAsteroid(Pht::ISceneManager& sceneManager, float scale) {
    Pht::Material asteroidMaterial {"brown_asteroid.jpg", 0.425f, 1.0f, 0.05f, 1.0f};
    mBigAsteroidRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"asteroid_998.obj", 30.0f * scale},
                                                                 asteroidMaterial);
}

void FloatingBlocks::InitBlocks(Pht::Scene& scene, float scale, float angularVelocity) {
    std::vector<FloatingBlockColor> colors {
        FloatingBlockColor::Red,
        FloatingBlockColor::Green,
        FloatingBlockColor::Blue,
        FloatingBlockColor::Gold
    };

    for (auto i = 0; i < mBlocks.size(); ++i) {
        const auto& volume = mVolumes[i];
    
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
        
        auto rotation =
            volume.mBlockRotation.HasValue() ? volume.mBlockRotation.GetValue() :
            Pht::Vec3 {
                Pht::NormalizedRand() * 360.0f,
                Pht::NormalizedRand() * 360.0f,
                Pht::NormalizedRand() * 360.0f,
            };

        auto blockAngularVelocity =
            volume.mBlockRotation.HasValue() ? Pht::Vec3{0.0f, 0.0f, 0.0f} :
            Pht::Vec3 {
                (Pht::NormalizedRand() - 0.5f) * angularVelocity,
                (Pht::NormalizedRand() - 0.5f) * angularVelocity,
                (Pht::NormalizedRand() - 0.5f) * angularVelocity
            };
        
        auto& block = mBlocks[i];
        block.mVelocity = velocity;
        block.mAngularVelocity = blockAngularVelocity;
        block.mElapsedTime = Pht::NormalizedRand() * rotationDuration;
        
        auto& renderable = CalcBlockRenderable(volume, colors);
        
        switch (volume.mPieceType) {
            case FloatingPieceType::BigSingleBlock:
                block.mSceneObject->SetRenderable(&renderable);
                break;
            case FloatingPieceType::SingleBlock: {
                auto blockSize = scale / 1.7f;
                auto blockScale = blockSize / scale;
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
            case FloatingPieceType::Bomb:
            case FloatingPieceType::RowBomb:
            case FloatingPieceType::Asteroid:
            case FloatingPieceType::BigAsteroid:
                block.mSceneObject->SetRenderable(&renderable);
                break;
        }
        
        auto& transform = block.mSceneObject->GetTransform();
        transform.SetPosition(position);
        transform.SetRotation(rotation);
    }
}

Pht::RenderableObject& FloatingBlocks::CalcBlockRenderable(const BlockPathVolume& volume,
                                                           std::vector<FloatingBlockColor>& colors) {
    switch (volume.mPieceType) {
        case FloatingPieceType::Bomb:
            return *mBombRenderable;
        case FloatingPieceType::RowBomb:
            return *mRowBombRenderable;
        case FloatingPieceType::Asteroid:
            return *mAsteroidRenderable;
        case FloatingPieceType::BigAsteroid:
            return *mBigAsteroidRenderable;
        default:
            break;
    }

    switch (volume.mBlockColor) {
        case FloatingBlockColor::Red:
        case FloatingBlockColor::Green:
        case FloatingBlockColor::Blue:
        case FloatingBlockColor::Gold:
        case FloatingBlockColor::Gray:
            return *mBlockRenderables[static_cast<int>(volume.mBlockColor)];
        case FloatingBlockColor::Random:
            return *mBlockRenderables[std::rand() % numBlockRenderables];
        case FloatingBlockColor::RandomExceptGray:
            return *mBlockRenderables[std::rand() % (numBlockRenderables - 1)];
        case FloatingBlockColor::RandomOneOfEachColorExceptGray: {
            assert(!colors.empty());
            auto colorIndex = std::rand() % colors.size();
            auto& renderable = *mBlockRenderables[static_cast<int>(colors[colorIndex])];
            colors.erase(std::begin(colors) + colorIndex);
            return renderable;
        }
    }
}

void FloatingBlocks::CreateLPiece(FloatingBlock& block,
                                  float scale,
                                  Pht::RenderableObject& renderable,
                                  Pht::Scene& scene) {
    auto blockSize = scale / 1.7f;
    auto blockScale = blockSize / scale;

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
    auto blockSize = scale / 1.7f;
    auto blockScale = blockSize / scale;

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
    auto blockSize = scale / 1.7f;
    auto blockScale = blockSize / scale;
    
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
    auto blockSize = scale / 1.7f;
    auto blockScale = blockSize / scale;

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
    auto dt = mEngine.GetLastFrameSeconds();
    
    AnimateEmissive(dt);

    for (auto i = 0; i < mBlocks.size(); ++i) {
        auto& block = mBlocks[i];
        auto& transform = block.mSceneObject->GetTransform();
        transform.Translate(block.mVelocity * dt);
        transform.Rotate(block.mAngularVelocity * dt);
        
        const auto& volume = mVolumes[i];
        auto rightLimit = volume.mPosition.x + volume.mSize.x / 2.0f;
        auto leftLimit = volume.mPosition.x - volume.mSize.x / 2.0f;
        auto position = block.mSceneObject->GetWorldSpacePosition();
        
        if (position.x > rightLimit && block.mVelocity.x > 0.0f) {
            block.mVelocity.x = -block.mVelocity.x;
        }
        
        if (position.x < leftLimit && block.mVelocity.x < 0.0f) {
            block.mVelocity.x = -block.mVelocity.x;
        }
        
        if (volume.mBlockRotation.HasValue()) {
            block.mElapsedTime += dt;
            if (block.mElapsedTime > rotationDuration) {
                block.mElapsedTime = 0.0f;
            }
            
            auto t = block.mElapsedTime * 2.0f * 3.1415f / rotationDuration;
            
            Pht::Vec3 rotation {
                rotationAmplitude * std::sin(t),
                rotationAmplitude * std::cos(t),
                0.0f
            };
            
            transform.SetRotation(rotation + volume.mBlockRotation.GetValue());
        }
    }
}

void FloatingBlocks::AnimateEmissive(float dt) {
    mEmissiveAnimationTime += dt;
    if (mEmissiveAnimationTime > emissiveAnimationDuration) {
        mEmissiveAnimationTime = 0.0f;
    }

    auto sineOfT = sin(mEmissiveAnimationTime * 2.0f * 3.1415f / emissiveAnimationDuration);
    auto emissive = emissiveAmplitude * (sineOfT + 1.0f) / 2.0f;
    
    Pht::SceneObjectUtils::SetEmissiveInRenderable(*mBombRenderable, emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(*mRowBombRenderable, emissive);
}
