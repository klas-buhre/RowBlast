#include "MapScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "CylinderMesh.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"
#include "ISceneManager.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UserServices.hpp"
#include "Universe.hpp"
#include "NextLevelParticleEffect.hpp"
#include "PortalParticleEffect.hpp"
#include "SunParticleEffect.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const auto halfMapWidth {22.0f};
    constexpr auto lightAnimationDuration {5.0f};
    const Pht::Vec3 lightDirectionA {0.57f, 1.0f, 0.6f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
    
    enum class Layer {
        Space,
        Map,
        Ufo,
        SunEffect,
        Hud,
        UiViews,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
    };

    const BackgroundLight& GetBackgroundLight(const World& world) {
        auto& backgroundLights {world.mBackgroundLights};
        
        assert(!backgroundLights.empty());
        return backgroundLights[std::rand() % backgroundLights.size()];
    }
}

MapScene::MapScene(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   UserServices& userServices,
                   const Universe& universe) :
    mEngine {engine},
    mUserServices {userServices},
    mCommonResources {commonResources},
    mUniverse {universe},
    mStarRenderable {
        engine.GetSceneManager().CreateRenderableObject(Pht::ObjMesh {"star.obj", 0.05f},
                                                        commonResources.GetMaterials().GetGoldMaterial())
    },
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(46)} {}

void MapScene::Init() {
    auto& world {mUniverse.GetWorld(mWorldId)};
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("mapScene"))};
    mScene = scene.get();
    
    mEngine.GetRenderer().EnableShader(Pht::ShaderType::TexturedEnvMapLighting);
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Space)});
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Map)});
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Ufo)});
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::SunEffect)});

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
    light.SetAmbientIntensity(world.mAmbientLightIntensity);
    light.SetDirectionalIntensity(world.mDirectionalLightIntensity);
    scene->GetRoot().AddChild(light.GetSceneObject());

    mCamera = &scene->CreateCamera();
    scene->GetRoot().AddChild(mCamera->GetSceneObject());
    
    auto& backgroundLight {GetBackgroundLight(world)};
    
    CreateWorld(world, backgroundLight);
    CreatePins(world);
    CreateEffects(world, backgroundLight);
    
    if (mClickedPortalNextLevelId.HasValue()) {
        SetCameraAtPortal(mClickedPortalNextLevelId.GetValue());
    } else {
        SetCameraAtLevel(mUserServices.GetProgressService().GetCurrentLevel());
    }
    
    mUfoContainer = &scene->CreateSceneObject();
    mUfoContainer->SetLayer(static_cast<int>(Layer::Ufo));
    scene->GetRoot().AddChild(*mUfoContainer);

    mHud = std::make_unique<MapHud>(mEngine,
                                    mUserServices,
                                    mCommonResources,
                                    *scene,
                                    static_cast<int>(Layer::Hud));
    
    mUiViewsContainer = &scene->CreateSceneObject();
    mUiViewsContainer->SetLayer(static_cast<int>(Layer::UiViews));
    mUiViewsContainer->AddChild(uiLightSceneObject);
    mUiViewsContainer->AddChild(uiCameraSceneObject);
    scene->GetRoot().AddChild(*mUiViewsContainer);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
    
    mClickedPortalNextLevelId = Pht::Optional<int> {};
}

void MapScene::CreateWorld(const World& world, const BackgroundLight& backgroundLight) {
    if (!world.mBackgroundTextureFilename.empty()) {
        Pht::Material backgroundMaterial {world.mBackgroundTextureFilename};
        
        auto& background {
            mScene->CreateSceneObject(Pht::QuadMesh {2400.0f, 2400.0f}, backgroundMaterial)
        };
        
        background.GetTransform().SetPosition({0.0f, 0.0f, -720.0f});
        background.SetLayer(static_cast<int>(Layer::Space));
        mScene->GetRoot().AddChild(background);
    }
    
    mPlanets = std::make_unique<Planets>(mEngine,
                                         *mScene,
                                         static_cast<int>(Layer::Space),
                                         world.mPlanets,
                                         backgroundLight.mDirection,
                                         backgroundLight.mIntensity);

    mClouds = std::make_unique<Clouds>(mEngine,
                                       *mScene,
                                       static_cast<int>(Layer::Map),
                                       world.mCloudPaths,
                                       world.mHazeLayers,
                                       1.5f,
                                       world.mCloudColor);

    mFloatingBlocks = std::make_unique<FloatingBlocks>(mEngine,
                                                       *mScene,
                                                       static_cast<int>(Layer::Map),
                                                       world.mBlockPaths,
                                                       mCommonResources,
                                                       2.0f,
                                                       20.0f);
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
    auto levelId {0};
    Pht::Vec3 position;
    
    switch (place.GetKind()) {
        case MapPlace::Kind::MapLevel: {
            auto& mapLevel {place.GetMapLevel()};
            levelId = mapLevel.mLevelId;
            position = mapLevel.mPosition;
            break;
        }
        case MapPlace::Kind::Portal: {
            auto& portal {place.GetPortal()};
            levelId = portal.mNextLevelId;
            position = portal.mPosition;
            break;
        }
    }
    
    auto& progressService {mUserServices.GetProgressService()};
    auto isClickable {levelId <= progressService.GetProgress()};
    
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
                                 levelId,
                                 progressService.GetNumStars(levelId),
                                 isClickable,
                                 place)
    };
    
    mPreviousPin = pin.get();
    mPins.push_back(std::move(pin));
}

void MapScene::CreateEffects(const World& world, const BackgroundLight& backgroundLight) {
    if (auto* pin {GetLevelPin(mUserServices.GetProgressService().GetProgress())}) {
        auto& pinPosition {pin->GetPosition()};
        CreateNextLevelParticleEffect(mEngine, *mScene, pinPosition, static_cast<int>(Layer::Map));
    }
    
    for (const auto& pin: mPins) {
        if (pin->GetPlace().GetKind() == MapPlace::Kind::Portal) {
            CreatePortalParticleEffect(mEngine,
                                       *mScene,
                                       pin->GetPosition(),
                                       static_cast<int>(Layer::Map));
        }
    }
    
    if (backgroundLight.mSun.HasValue()) {
        CreateSunParticleEffect(mEngine,
                                *mScene,
                                backgroundLight.mSun.GetValue(),
                                static_cast<int>(Layer::SunEffect));
    }
}

void MapScene::Update() {
    mPlanets->Update();
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

void MapScene::SetCameraAtLevel(int levelId) {
    if (auto* pin {GetLevelPin(levelId)}) {
        SetCameraXPosition(pin->GetPosition().x);
    }
}

void MapScene::SetCameraBetweenLevels(int levelIdA, int levelIdB) {
    auto* pinA {GetPin(levelIdA)};
    auto* pinB {GetPin(levelIdB)};
    
    if (pinA && pinB) {
        SetCameraXPosition((pinA->GetPosition().x + pinB->GetPosition().x) / 2.0f);
    }
}

void MapScene::SetCameraAtPortal(int portalNextLevelId) {
     if (auto* pin {GetPortalPin(portalNextLevelId)}) {
        SetCameraXPosition(pin->GetPosition().x);
    }
}

float MapScene::GetCameraXPosition() const {
    return mCamera->GetSceneObject().GetTransform().GetPosition().x;
}

const MapPin* MapScene::GetPin(int id) const {
    for (const auto& pin: mPins) {
        if (pin->GetLevel() == id) {
            return pin.get();
        }
    }
    
    return nullptr;
}

const MapPin* MapScene::GetLevelPin(int levelId) const {
    for (const auto& pin: mPins) {
        switch (pin->GetPlace().GetKind()) {
            case MapPlace::Kind::Portal:
                break;
            case MapPlace::Kind::MapLevel:
                if (pin->GetPlace().GetMapLevel().mLevelId == levelId) {
                    return pin.get();
                }
                break;
        }
    }
    
    return nullptr;
}

const MapPin* MapScene::GetPortalPin(int portalNextLevelId) const {
    for (const auto& pin: mPins) {
        switch (pin->GetPlace().GetKind()) {
            case MapPlace::Kind::Portal:
                if (pin->GetPlace().GetPortal().mNextLevelId == portalNextLevelId) {
                    return pin.get();
                }
                break;
            case MapPlace::Kind::MapLevel:
                break;
        }
    }
    
    return nullptr;
}
