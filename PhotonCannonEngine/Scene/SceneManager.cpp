#include "SceneManager.hpp"

#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Renderer.hpp"
#include "RenderableObject.hpp"
#include "CameraComponent.hpp"
#include "TextComponent.hpp"

using namespace Pht;

SceneManager::SceneManager(Renderer& renderer) :
    mRenderer {renderer} {}

SceneManager::~SceneManager() {}

void SceneManager::SetLoadedScene(std::unique_ptr<Scene> scene) {
    mScene = std::move(scene);
    mScene->GetRenderQueue().Allocate();
}

Scene* SceneManager::GetScene() {
    return mScene.get();
}

std::unique_ptr<RenderableObject> SceneManager::CreateRenderableObject(const IMesh& mesh,
                                                                       const Material& material) {
    return mRenderer.CreateRenderableObject(mesh, material);
}

std::unique_ptr<SceneObject> SceneManager::CreateCamera() {
    auto sceneObject {std::make_unique<SceneObject>()};
    auto cameraComponent {std::make_unique<CameraComponent>(*sceneObject)};
    
    sceneObject->SetComponent<CameraComponent>(std::move(cameraComponent));
    return sceneObject;
}

std::unique_ptr<SceneObject> SceneManager::CreateText(const std::string& text,
                                                      const TextProperties& properties) {
    auto sceneObject {std::make_unique<SceneObject>()};
    auto textComponent {std::make_unique<TextComponent>(*sceneObject, text, properties)};
    
    sceneObject->SetComponent<TextComponent>(std::move(textComponent));
    return sceneObject;
}
