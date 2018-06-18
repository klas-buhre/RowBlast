#include "MapScene.hpp"

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
#include "NextLevelParticleEffect.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const auto halfMapWidth {22.0f};
    constexpr auto lightAnimationDuration {5.0f};
    const Pht::Vec3 lightDirectionA {0.57f, 1.0f, 0.6f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
    
    enum class Layer {
        Map,
        Hud,
        UiViews,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
    };

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {120.0f, 0.0f, 0.0f},
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -60.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -30.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 40.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f}
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -100.0f},
            .mSize = {250.0f, 180.0f, 20.0f},
            .mCloudSize = {60.0f, 60.0f},
            .mCloudSizeRandPart = 60.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {400.0f, 320.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {510.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -400.0f},
            .mSize = {510.0f, 400.0f, 20.0f},
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
            .mUpperColor = {0.17f, 0.38f, 0.89f, 1.0f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 500.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 380.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 0.35f}
        }
    };
}

MapScene::MapScene(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   UserData& userData) :
    mEngine {engine},
    mUserData {userData},
    mCommonResources {commonResources},
    mStarRenderable {
        engine.GetSceneManager().CreateRenderableObject(Pht::ObjMesh {"star.obj", 0.05f},
                                                        commonResources.GetMaterials().GetGoldMaterial())
    },
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(46)} {}

void MapScene::Init() {
    CreateScene(GetChapter(1));
}

void MapScene::CreateScene(const Chapter& chapter) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("mapScene"))};
    mScene = scene.get();
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Map)});
    Pht::RenderPass hudRenderPass {static_cast<int>(Layer::Hud)};
    hudRenderPass.SetHudMode(true);
    scene->AddRenderPass(hudRenderPass);
    
    Pht::RenderPass uiViewsRenderPass {static_cast<int>(Layer::UiViews)};
    uiViewsRenderPass.SetHudMode(true);
    uiViewsRenderPass.SetIsDepthTestAllowed(false);
    uiViewsRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);

    auto& uiLightSceneObject {scene->CreateSceneObject()};
    uiLightSceneObject.SetIsVisible(false);
    auto uiLightComponent {std::make_unique<Pht::LightComponent>(uiLightSceneObject)};
    mUiLight = uiLightComponent.get();
    uiViewsRenderPass.SetLight(mUiLight);
    uiLightSceneObject.SetComponent<Pht::LightComponent>(std::move(uiLightComponent));

    auto& uiCameraSceneObject {scene->CreateSceneObject()};
    uiCameraSceneObject.SetIsVisible(false);
    uiCameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 300.0f});
    auto uiCameraComponent {std::make_unique<Pht::CameraComponent>(uiCameraSceneObject)};
    uiViewsRenderPass.SetCamera(uiCameraComponent.get());
    uiCameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(uiCameraComponent));

    scene->AddRenderPass(uiViewsRenderPass);

    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
    scene->GetRoot().AddChild(light.GetSceneObject());

    mCamera = &scene->CreateCamera();
    scene->GetRoot().AddChild(mCamera->GetSceneObject());
    
    mClouds = std::make_unique<Clouds>(mEngine,
                                       *scene,
                                       static_cast<int>(Layer::Map),
                                       cloudPaths,
                                       hazeLayers,
                                       1.5f);

    mFloatingBlocks = std::make_unique<FloatingBlocks>(mEngine,
                                                       *scene,
                                                       static_cast<int>(Layer::Map),
                                                       chapter.mBlockPaths,
                                                       mCommonResources,
                                                       1.5f,
                                                       20.0f);
    
    CreatePins(chapter);
    SetCameraAtCurrentLevel();
    
    auto* pin {mPins[mUserData.GetProgressManager().GetProgress() - 1].get()};
    auto& pinPosition {pin->GetPosition()};
    CreateNextLevelParticleEffect(mEngine, *scene, pinPosition, static_cast<int>(Layer::Map));
    
    mHud = std::make_unique<MapHud>(mEngine,
                                    mUserData,
                                    mCommonResources.GetHussarFontSize22(PotentiallyZoomedScreen::No),
                                    *scene,
                                    static_cast<int>(Layer::Hud));
    
    mUiViewsContainer = &scene->CreateSceneObject();
    mUiViewsContainer->SetLayer(static_cast<int>(Layer::UiViews));
    mUiViewsContainer->AddChild(uiLightSceneObject);
    mUiViewsContainer->AddChild(uiCameraSceneObject);
    scene->GetRoot().AddChild(*mUiViewsContainer);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
}

void MapScene::CreatePins(const Chapter& chapter) {
    mPinsContainer = &mScene->CreateSceneObject();
    mPinsContainer->SetLayer(static_cast<int>(Layer::Map));
    mScene->GetRoot().AddChild(*mPinsContainer);
    
    mPreviousPin = nullptr;
    mPins.clear();
    
    for (auto& level: chapter.mLevels) {
        CreatePin(*mPinsContainer, level.mLevelIndex, level.mPosition);
    }
}

void MapScene::CreatePin(Pht::SceneObject& pinsContainerObject,
                         int level,
                         const Pht::Vec3& position) {
    auto& progressManager {mUserData.GetProgressManager()};
    auto isClickable {level <= progressManager.GetProgress()};
    
    if (mPreviousPin) {
        const auto& connectionMaterial {
            isClickable ? mCommonResources.GetMaterials().GetBlueMaterial() :
            mCommonResources.GetMaterials().GetLightGrayMaterial()
        };
        
        auto pinPositionDiff {position - mPreviousPin->GetPosition()};
        
        auto connectionZAngle {
            Pht::ToDegrees(std::atan(pinPositionDiff.y / pinPositionDiff.x)) - 90.0f
        };

        auto connectionXAngle {
            Pht::ToDegrees(std::atan(pinPositionDiff.z / pinPositionDiff.x))
        };

        Pht::Vec3 connectionPosition {
            mPreviousPin->GetPosition().x + pinPositionDiff.x / 2.0f,
            mPreviousPin->GetPosition().y + pinPositionDiff.y / 2.0f,
            mPreviousPin->GetPosition().z + pinPositionDiff.z / 2.0f,
        };
        
        Pht::CylinderMesh connectionMesh {0.3f, 4.0f, false, std::string{"mapConnection"}};
        auto& connection {mScene->CreateSceneObject(connectionMesh, connectionMaterial)};
        auto& transform {connection.GetTransform()};
        transform.SetRotation({connectionXAngle, 0.0f, connectionZAngle});
        transform.SetPosition(connectionPosition);
        pinsContainerObject.AddChild(connection);
    }
    
    auto pin {
        std::make_unique<MapPin>(mEngine,
                                 mCommonResources,
                                 mFont,
                                 *mScene,
                                 pinsContainerObject,
                                 *mStarRenderable,
                                 position,
                                 level,
                                 progressManager.GetNumStars(level),
                                 isClickable)
    };
    
    mPreviousPin = pin.get();
    mPins.push_back(std::move(pin));
}

void MapScene::Update() {
    mClouds->Update();
    mFloatingBlocks->Update();
    mHud->Update();
    UpdateUiLightAnimation();
}

void MapScene::UpdateUiLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    auto t {(cos(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f};
    mUiLight->SetDirection(lightDirectionA.Lerp(t, lightDirectionB));
}

void MapScene::SetCameraXPosition(float xPosition) {
    if (xPosition < -halfMapWidth) {
        xPosition = -halfMapWidth;
    } else if (xPosition > halfMapWidth) {
        xPosition = halfMapWidth;
    }
    
    Pht::Vec3 position {xPosition, 0.0f, 20.0f};
    mCamera->GetSceneObject().GetTransform().SetPosition(position);
    
    Pht::Vec3 target {position.x, position.y, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera->SetTarget(target, up);
}

void MapScene::SetCameraAtCurrentLevel() {
    auto* pin {mPins[mUserData.GetProgressManager().GetCurrentLevel() - 1].get()};
    SetCameraXPosition(pin->GetPosition().x);
}

float MapScene::GetCameraXPosition() const {
    return mCamera->GetSceneObject().GetTransform().GetPosition().x;
}
