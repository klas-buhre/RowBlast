#include "GameHudNew.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"

using namespace BlocksGame;

GameHudNew::GameHudNew(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                GameHudController& gameHudController,
                const Pht::Font& font,
                Pht::Scene& scene,
                Pht::SceneObject& parentObject,
                int hudLayer) :
    mEngine {engine},
    mGameLogic {gameLogic} {

    CreateLightAndCamera(scene, parentObject, hudLayer);
}

void GameHudNew::CreateLightAndCamera(Pht::Scene& scene,
                                      Pht::SceneObject& parentObject,
                                      int hudLayer) {
    auto& lightSceneObject {scene.CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    mLight = lightComponent.get();
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    parentObject.AddChild(lightSceneObject);
    
    auto& cameraSceneObject {scene.CreateSceneObject()};
    cameraSceneObject.SetIsVisible(false);
    cameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 15.5f});
    auto cameraComponent {std::make_unique<Pht::CameraComponent>(cameraSceneObject)};
    auto* camera {cameraComponent.get()};
    cameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(cameraComponent));
    parentObject.AddChild(cameraSceneObject);
    
    auto* hudRenderPass {scene.GetRenderPass(hudLayer)};
    assert(hudRenderPass);
    hudRenderPass->SetLight(mLight);
    hudRenderPass->SetCamera(camera);
}

void GameHudNew::OnSwitchButtonDown() {

}

void GameHudNew::OnSwitchButtonUp() {

}
