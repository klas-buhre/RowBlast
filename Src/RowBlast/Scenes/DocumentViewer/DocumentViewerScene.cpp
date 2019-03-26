#include "DocumentViewerScene.hpp"

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
        ScrollPanel,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
    };
}

DocumentViewerScene::DocumentViewerScene(Pht::IEngine& engine) :
    mEngine {engine} {}

void DocumentViewerScene::Init() {
    auto& sceneManager = mEngine.GetSceneManager();
    auto scene = sceneManager.CreateScene(Pht::Hash::Fnv1a("documentViewerScene"));
    mScene = scene.get();
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    uiRenderPass.SetIsDepthTestAllowed(false);
    uiRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);
    scene->AddRenderPass(uiRenderPass);

    Pht::RenderPass scrollPanelRenderPass {static_cast<int>(Layer::ScrollPanel)};
    scrollPanelRenderPass.SetHudMode(true);
    scrollPanelRenderPass.SetIsDepthTestAllowed(false);
    scrollPanelRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);
    Pht::ScissorBox scissorBox {{-7.5f, -10.0f}, {15.0f, 20.0}};
    scrollPanelRenderPass.SetScissorBox(scissorBox);
    scene->AddRenderPass(scrollPanelRenderPass);

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
    
    mScrollPanelContainer = &scene->CreateSceneObject();
    mScrollPanelContainer->SetLayer(static_cast<int>(Layer::ScrollPanel));
    scene->GetRoot().AddChild(*mScrollPanelContainer);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}
