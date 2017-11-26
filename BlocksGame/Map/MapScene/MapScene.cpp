#include "MapScene.hpp"

#include <sstream>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "CylinderMesh.hpp"
#include "SphereMesh.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "MapLoader.hpp"
#include "CommonResources.hpp"
#include "UserData.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec2 sceneSize {65.0f, 65.0f};
    const auto cubePathYSpacing {1.5f};
    const auto cubePathYSize {0.5f};
    const auto numCubes {static_cast<int>(sceneSize.y / cubePathYSpacing)};
    
    const std::vector<Volume> foregroundCubePaths {
        Volume {
            .mPosition = {-22.0f, -16.5f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {-17.0f, -17.0f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {-12.0f, -17.5f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {-7.0f, -18.0f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        }
    };
}

MapScene::MapScene(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   UserData& userData) :
    mEngine {engine},
    mUserData {userData},
    mNextLevelParticleEffect {engine},
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(35)} {

    CreateBackground(commonResources.GetMaterials().GetSkyMaterial());
    CreateFloatingCubes(commonResources);
    LoadStar(commonResources);
    CreatePins(commonResources);
}

void MapScene::Reset() {
    mEngine.GetRenderer().SetLightDirection({1.0f, 1.0f, 1.0f});
    mFloatingCubes->Reset();
    ReflectProgressInPinsAndConnections();
    StartParticleEffect();
    SetCameraAtCurrentLevel();
}

void MapScene::Stop() {
    mNextLevelParticleEffect.Stop();
}

void MapScene::Update() {
    mFloatingCubes->Update();
    
    for (auto& pin: mPins) {
        pin->Update();
    }
}

void MapScene::SetCameraPosition(Pht::Vec3 position) {
    auto& frustumSize {mEngine.GetRenderer().GetOrthographicFrustumSize()};
    auto halfFrustumWidth {frustumSize.x / 2.0f};
    auto halfMapWidth {mMapSize.x / 2.0f};
    
    if (position.x - halfFrustumWidth < -halfMapWidth) {
        position.x = -halfMapWidth + halfFrustumWidth;
    } else if (position.x + halfFrustumWidth > halfMapWidth) {
        position.x = halfMapWidth - halfFrustumWidth;
    }

    auto halfFrustumHeight {frustumSize.y / 2.0f};
    auto halfMapHeight {mMapSize.y / 2.0f};
    if (position.y - halfFrustumHeight < -halfMapHeight) {
        position.y = -halfMapHeight + halfFrustumHeight;
    } else if (position.y + halfFrustumHeight > halfMapHeight) {
        position.y = halfMapHeight - halfFrustumHeight;
    }

    Pht::Vec3 target {position.x, position.y, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    
    mCameraPosition = position;
    mEngine.GetRenderer().LookAt(position, target, up);
}

const std::vector<FloatingCube>& MapScene::GetFloatingCubes() const {
    return mFloatingCubes->GetCubes();
}

const Pht::SceneObject& MapScene::GetNextLevelParticleEffect() const {
    return mNextLevelParticleEffect.GetSceneObject();
}

void MapScene::CreateBackground(const Pht::Material& backgroundMaterial) {
    mBackground = mEngine.GetSceneManager().CreateSceneObject(Pht::QuadMesh {sceneSize.x, sceneSize.y},
                                                              backgroundMaterial);
    mBackground->SetPosition({0.0f, 0.0f, -5.0f});
}

void MapScene::CreateFloatingCubes(const CommonResources& commonResources) {
    std::vector<Volume> floatingCubePaths;
    
    for (auto i {0}; i < numCubes; ++i) {
        Volume volume {
            .mPosition = {0.0f, sceneSize.y / 2.0f - i * cubePathYSpacing, -3.0f},
            .mSize = {sceneSize.x, cubePathYSize, 3.0f}
        };
        
        floatingCubePaths.push_back(volume);
    }
    
    for (auto& volume: foregroundCubePaths) {
        floatingCubePaths.push_back(volume);
    }
    
    mFloatingCubes = std::make_unique<FloatingCubes>(floatingCubePaths, mEngine, commonResources, 1.5f);
}

void MapScene::LoadStar(const CommonResources& commonResources) {
    mStarRenderable = mEngine.GetSceneManager().CreateRenderableObject(
        Pht::ObjMesh {"star.obj", 0.05f}, commonResources.GetMaterials().GetGoldMaterial());
}

void MapScene::CreatePins(const CommonResources& commonResources) {
    auto& sceneManager {mEngine.GetSceneManager()};
    
    mBluePinRenderable =
        sceneManager.CreateRenderableObject(Pht::SphereMesh {0.85f},
                                            commonResources.GetMaterials().GetBlueMaterial());
    
    mBlueConnectionRenderable =
        sceneManager.CreateRenderableObject(Pht::CylinderMesh {0.3f, 4.0f},
                                            commonResources.GetMaterials().GetBlueMaterial());
    
    mGrayPinRenderable =
        sceneManager.CreateRenderableObject(Pht::SphereMesh {0.85f},
                                            commonResources.GetMaterials().GetLightGrayMaterial());
    
    mGrayConnectionRenderable =
        sceneManager.CreateRenderableObject(Pht::CylinderMesh {0.3f, 4.0f},
                                            commonResources.GetMaterials().GetLightGrayMaterial());

    std::vector<MapLevel> levels;
    MapLoader::Load("map.json", levels);
    
    for (const auto& level: levels) {
        CreatePin(level.mLevelIndex, level.mPosition);
    }
}

void MapScene::CreatePin(int level, const Pht::Vec3& position) {
    Pht::TextProperties textProperties {
        mFont,
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        Pht::Vec2{0.05f, 0.05f},
        Pht::Vec4{0.4f, 0.4f, 0.4f, 0.5f}
    };
    
    Pht::Text text {{}, std::to_string(level), textProperties};
    auto pin {std::make_unique<MapPin>(mGrayPinRenderable, position, level, text, mEngine)};
    
    if (mPreviousPin) {
        auto pinPositionDiff {position - mPreviousPin->GetPosition()};
        
        auto connectionAngle {
            Pht::ToDegrees(std::atan(pinPositionDiff.y / pinPositionDiff.x)) + 90.0f
        };
        
        Pht::Vec3 connectionPosition {
            mPreviousPin->GetPosition().x + pinPositionDiff.x / 2.0f,
            mPreviousPin->GetPosition().y + pinPositionDiff.y / 2.0f,
            mPreviousPin->GetPosition().z + pinPositionDiff.z / 2.0f,
        };
        
        auto connection {std::make_unique<Pht::SceneObject>(mGrayConnectionRenderable)};
        connection->SetRotationZ(-connectionAngle);
        connection->SetPosition(connectionPosition);
        mConnections.push_back(std::move(connection));
    }
    
    mPreviousPin = pin.get();
    mPins.push_back(std::move(pin));
}

void MapScene::ReflectProgressInPinsAndConnections() {
    auto& progressManager {mUserData.GetProgressManager()};
    auto progress {progressManager.GetProgress()};
    
    for (auto levelIndex {0}; levelIndex < progress; ++levelIndex) {
        auto* pin {mPins[levelIndex].get()};
        pin->GetSceneObject().SetRenderable(mBluePinRenderable);
        pin->SetNumStars(progressManager.GetNumStars(levelIndex + 1), mStarRenderable);
        pin->SetIsClickable(true);
        
        if (levelIndex < progress - 1) {
            mConnections[levelIndex]->SetRenderable(mBlueConnectionRenderable);
        }
    }
}

void MapScene::StartParticleEffect() {
    auto* pin {mPins[mUserData.GetProgressManager().GetProgress() - 1].get()};
    auto& pinPosition {pin->GetPosition()};
    mNextLevelParticleEffect.StartEffect({pinPosition.x, pinPosition.y, 10.0f});
}

void MapScene::SetCameraAtCurrentLevel() {
    auto* pin {mPins[mUserData.GetProgressManager().GetCurrentLevel() - 1].get()};
    auto& pinPosition {pin->GetPosition()};
    SetCameraPosition({pinPosition.x, pinPosition.y, 20.0f});
}
