#include "MapSceneNew.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "CylinderMesh.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"
#include "ISceneManager.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UserData.hpp"
#include "Chapter.hpp"
#include "NextLevelParticleEffectNew.hpp"

using namespace BlocksGame;

namespace {
    enum class Layer {
        Map = 0,
        Hud = 1
    };

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {75.0f, 0.0f, 0.0f},
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -50.0f},
            .mSize = {140.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -60.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -30.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 40.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f}
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -100.0f},
            .mSize = {210.0f, 180.0f, 20.0f},
            .mCloudSize = {60.0f, 60.0f},
            .mCloudSizeRandPart = 60.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {380.0f, 320.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {500.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -400.0f},
            .mSize = {500.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        }
    };
    
    const std::vector<HazeLayer> hazeLayers {
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -700.0f},
            .mSize = {1500.0f, 1050.0f},
            .mUpperColor = {0.17f, 0.34f, 1.0f, 1.0f},
            .mLowerColor = {0.55f, 0.76f, 1.0f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 500.0f},
            .mUpperColor = {0.17f, 0.34f, 1.0f, 0.35f},
            .mLowerColor = {0.55f, 0.76f, 1.0f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 380.0f},
            .mUpperColor = {0.17f, 0.34f, 1.0f, 0.35f},
            .mLowerColor = {0.55f, 0.76f, 1.0f, 0.35f}
        }
    };
}

MapSceneNew::MapSceneNew(Pht::IEngine& engine,
                         const CommonResources& commonResources,
                         UserData& userData) :
    mEngine {engine},
    mUserData {userData},
    mCommonResources {commonResources},
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(35)},
    mStarRenderable {
        engine.GetSceneManager().CreateRenderableObject(Pht::ObjMesh {"star.obj", 0.05f},
                                                        commonResources.GetMaterials().GetGoldMaterial())
    } {}

void MapSceneNew::Reset() {
    CreateScene(GetChapter(1));
}

void MapSceneNew::CreateScene(const Chapter& chapter) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("mapScene"))};
    mScene = scene.get();
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Map)});
    Pht::RenderPass hudRenderPass {static_cast<int>(Layer::Hud)};
    hudRenderPass.SetHudMode(true);
    scene->AddRenderPass(hudRenderPass);
    
    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
    scene->GetRoot().AddChild(light.GetSceneObject());

    mCamera = &scene->CreateCamera();
    scene->GetRoot().AddChild(mCamera->GetSceneObject());
    SetCameraAtCurrentLevel();
    
    mClouds = std::make_unique<Clouds>(mEngine,
                                       *scene,
                                       static_cast<int>(Layer::Map),
                                       cloudPaths,
                                       hazeLayers,
                                       2.0f);

    mFloatingCubes = std::make_unique<FloatingCubes>(mEngine,
                                                     scene.get(),
                                                     static_cast<int>(Layer::Map),
                                                     chapter.mCubePaths,
                                                     mCommonResources,
                                                     1.5f);
    
    CreatePins(chapter);
    
    auto* pin {mPins[mUserData.GetProgressManager().GetProgress() - 1].get()};
    auto& pinPosition {pin->GetPosition()};
    CreateNextLevelParticleEffect(mEngine, *scene, pinPosition, static_cast<int>(Layer::Map));
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceZ);
    sceneManager.SetLoadedScene(std::move(scene));
}

void MapSceneNew::CreatePins(const Chapter& chapter) {
    auto& pinContainerObject {mScene->CreateSceneObject()};
    pinContainerObject.SetLayer(static_cast<int>(Layer::Map));
    mScene->GetRoot().AddChild(pinContainerObject);
    
    mPreviousPin = nullptr;
    
    for (auto& level: chapter.mLevels) {
        CreatePin(pinContainerObject, level.mLevelIndex, level.mPosition);
    }
}

void MapSceneNew::CreatePin(Pht::SceneObject& pinContainerObject,
                            int level,
                            const Pht::Vec3& position) {
    auto& progressManager {mUserData.GetProgressManager()};
    auto isClickable {level < progressManager.GetProgress()};
    
    if (mPreviousPin) {
        const auto& connectionMaterial {
            isClickable ? mCommonResources.GetMaterials().GetBlueMaterial() :
            mCommonResources.GetMaterials().GetLightGrayMaterial()
        };
        
        auto pinPositionDiff {position - mPreviousPin->GetPosition()};
        
        auto connectionAngle {
            -Pht::ToDegrees(std::atan(pinPositionDiff.y / pinPositionDiff.x)) + 90.0f
        };
        
        Pht::Vec3 connectionPosition {
            mPreviousPin->GetPosition().x + pinPositionDiff.x / 2.0f,
            mPreviousPin->GetPosition().y + pinPositionDiff.y / 2.0f,
            mPreviousPin->GetPosition().z + pinPositionDiff.z / 2.0f,
        };
        
        Pht::CylinderMesh connectionMesh {0.3f, 4.0f, std::string{"mapConnection"}};
        auto& connection {mScene->CreateSceneObject(connectionMesh, connectionMaterial)};
        auto& transform {connection.GetTransform()};
        transform.SetRotation({0.0f, 0.0f, -connectionAngle});
        transform.SetPosition(connectionPosition);
        pinContainerObject.AddChild(connection);
    }
    
    auto pin {
        std::make_unique<MapPinNew>(mEngine,
                                    mCommonResources,
                                    mFont,
                                    *mScene,
                                    pinContainerObject,
                                    mStarRenderable,
                                    position,
                                    level,
                                    progressManager.GetNumStars(level + 1),
                                    isClickable)
    };
    
    mPreviousPin = pin.get();
    mPins.push_back(std::move(pin));
}

void MapSceneNew::Update() {
    mClouds->Update();
    mFloatingCubes->Update();
}

void MapSceneNew::SetCameraPosition(float xPosition) {
    auto halfMapWidth {mMapSizeX / 2.0f};
    
    if (xPosition < -halfMapWidth) {
        xPosition = -halfMapWidth;
    } else if (xPosition > halfMapWidth) {
        xPosition = halfMapWidth;
    }
    
    Pht::Vec3 position {xPosition, 0.0f, 20.0f};
    mCamera->GetSceneObject().GetTransform().SetPosition(position);
    
    Pht::Vec3 target {xPosition, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera->SetTarget(target, up);
}

void MapSceneNew::SetCameraAtCurrentLevel() {
    auto* pin {mPins[mUserData.GetProgressManager().GetCurrentLevel() - 1].get()};
    SetCameraPosition(pin->GetPosition().x);
}