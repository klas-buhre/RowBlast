#include "SceneManager.hpp"

#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Renderer.hpp"
#include "RenderableObject.hpp"
#include "Fnv1Hash.hpp"

using namespace Pht;

SceneManager::SceneManager(Renderer& renderer) :
    mRenderer {renderer} {}

SceneManager::~SceneManager() {}

std::unique_ptr<Scene> SceneManager::CreateScene(Scene::Name name) {
    return std::make_unique<Scene>(*this, name);
}

void SceneManager::SetLoadedScene(std::unique_ptr<Scene> scene) {
    mScene = std::move(scene);
    
    if (mScene) {
        mScene->GetRenderQueue().Allocate();
    }
}

Scene* SceneManager::GetActiveScene() {
    return mScene.get();
}

std::unique_ptr<RenderableObject> SceneManager::CreateRenderableObject(const IMesh& mesh,
                                                                       const Material& material) {
    return mRenderer.CreateRenderableObject(mesh, material);
}

std::unique_ptr<SceneObject> SceneManager::CreateSceneObject(const IMesh& mesh,
                                                             const Material& material) {
    auto renderableObject {mRenderer.CreateRenderableObject(mesh, material)};
    return std::make_unique<SceneObject>(std::move(renderableObject));
}
