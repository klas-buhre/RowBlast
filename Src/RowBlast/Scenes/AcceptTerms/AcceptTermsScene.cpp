#include "AcceptTermsScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "SceneObject.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    enum class Layer {
        Ui,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
    };
}

AcceptTermsScene::AcceptTermsScene(Pht::IEngine& engine) :
    mEngine {engine} {}
    
void AcceptTermsScene::Init() {
    auto& sceneManager = mEngine.GetSceneManager();
    auto scene = sceneManager.CreateScene(Pht::Hash::Fnv1a("acceptTermsScene"));
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    uiRenderPass.SetIsDepthTestAllowed(false);
    uiRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);
    scene->AddRenderPass(uiRenderPass);
    
    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto& light = scene->CreateGlobalLight();
    scene->GetRoot().AddChild(light.GetSceneObject());

    auto& camera = scene->CreateCamera();
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    camera.GetSceneObject().GetTransform().SetPosition(cameraPosition);
    camera.SetTarget(target, up);
    scene->GetRoot().AddChild(camera.GetSceneObject());
    
    mUiViewsContainer = &scene->CreateSceneObject();
    mUiViewsContainer->SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(*mUiViewsContainer);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}
