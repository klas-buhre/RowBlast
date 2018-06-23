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
#include "World.hpp"
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
        Avatar,
        Hud,
        UiViews,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
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
    CreateScene(GetWorld(1));
}

void MapScene::CreateScene(const World& world) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("mapScene"))};
    mScene = scene.get();
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Map)});
    
    Pht::RenderPass avatarRenderPass {static_cast<int>(Layer::Avatar)};
    avatarRenderPass.SetIsDepthTestAllowed(false);
    avatarRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);
    mScene->AddRenderPass(avatarRenderPass);

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
                                       world.mCloudPaths,
                                       world.mHazeLayers,
                                       1.5f);

    mFloatingBlocks = std::make_unique<FloatingBlocks>(mEngine,
                                                       *scene,
                                                       static_cast<int>(Layer::Map),
                                                       world.mBlockPaths,
                                                       mCommonResources,
                                                       1.5f,
                                                       20.0f);
    
    CreatePins(world);
    SetCameraAtLevel(mUserData.GetProgressManager().GetCurrentLevel());
    
    mAvatarContainer = &scene->CreateSceneObject();
    mAvatarContainer->SetLayer(static_cast<int>(Layer::Avatar));
    scene->GetRoot().AddChild(*mAvatarContainer);
    
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

void MapScene::CreatePins(const World& world) {
    auto& pinsContainer {mScene->CreateSceneObject()};
    pinsContainer.SetLayer(static_cast<int>(Layer::Map));
    mScene->GetRoot().AddChild(pinsContainer);
    
    mPreviousPin = nullptr;
    mPins.clear();
    
    for (auto& place: world.mPlaces) {
        CreatePin(pinsContainer, place);
    }
}

void MapScene::CreatePin(Pht::SceneObject& pinsContainerObject, const MapPlace& place) {
    auto level {0};
    Pht::Vec3 position;
    
    switch (place.GetKind()) {
        case MapPlace::Kind::MapLevel: {
            auto& mapLevel {place.GetMapLevel()};
            level = mapLevel.mLevelIndex;
            position = mapLevel.mPosition;
            break;
        }
        case MapPlace::Kind::Portal: {
            auto& portal {place.GetPortal()};
            level = portal.mLevelIndexEquivalent;
            position = portal.mPosition;
            break;
        }
    }
    
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
                                 isClickable,
                                 place)
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

void MapScene::SetCameraAtLevel(int levelIndex) {
    auto& pin {*mPins[levelIndex - 1]};
    SetCameraXPosition(pin.GetPosition().x);
}

void MapScene::SetCameraBetweenLevels(int levelA, int levelB) {
    auto& pinA {*mPins[levelA - 1]};
    auto& pinB {*mPins[levelB - 1]};
    SetCameraXPosition((pinA.GetPosition().x + pinB.GetPosition().x) / 2.0f);
}

float MapScene::GetCameraXPosition() const {
    return mCamera->GetSceneObject().GetTransform().GetPosition().x;
}
