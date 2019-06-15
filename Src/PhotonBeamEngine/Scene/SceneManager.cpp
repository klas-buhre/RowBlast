#include "SceneManager.hpp"

#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Renderer.hpp"
#include "RenderableObject.hpp"
#include "Fnv1Hash.hpp"
#include "InputHandler.hpp"
#include "StaticBatcher.hpp"

using namespace Pht;

SceneManager::SceneManager(IRenderSystem& renderer, InputHandler& inputHandler) :
    mRenderer {renderer},
    mInputHandler {inputHandler} {}

SceneManager::~SceneManager() {}

std::unique_ptr<Scene> SceneManager::CreateScene(Scene::Name name) {
    return std::make_unique<Scene>(*this, name);
}

void SceneManager::InitSceneSystems(float narrowFrustumHeightFactor) {
    mRenderer.InitCamera(narrowFrustumHeightFactor);
    mInputHandler.Init(mRenderer);
}

void SceneManager::InitRenderer() {
    if (mScene) {
        mRenderer.InitRenderQueue(*mScene);
    }
}

void SceneManager::SetLoadedScene(std::unique_ptr<Scene> scene) {
    mScene = std::move(scene);
    InitRenderer();
}

Scene* SceneManager::GetActiveScene() {
    return mScene.get();
}

std::unique_ptr<RenderableObject> SceneManager::CreateRenderableObject(const IMesh& mesh,
                                                                       const Material& material) {
    return mRenderer.CreateRenderableObject(mesh, material, VertexBufferLocation::AtGpuOnly);
}

std::unique_ptr<RenderableObject>
SceneManager::CreateBatchableRenderableObject(const IMesh& mesh,
                                              const Material& material) {
    return mRenderer.CreateRenderableObject(mesh, material, VertexBufferLocation::AtGpuAndCpu);
}

std::unique_ptr<RenderableObject>
SceneManager::CreateStaticBatchRenderable(const SceneObject& sceneObject,
                                          const Optional<std::string>& batchVertexBufferName) {
    return StaticBatcher::CreateBatch(sceneObject, batchVertexBufferName);
}

std::unique_ptr<SceneObject> SceneManager::CreateSceneObject(const IMesh& mesh,
                                                             const Material& material,
                                                             SceneResources& sceneResources) {
    auto renderableObject = mRenderer.CreateRenderableObject(mesh,
                                                             material,
                                                             VertexBufferLocation::AtGpuOnly);
    auto sceneObject = std::make_unique<SceneObject>(renderableObject.get());
    sceneResources.AddRenderableObject(std::move(renderableObject));
    return sceneObject;
}
