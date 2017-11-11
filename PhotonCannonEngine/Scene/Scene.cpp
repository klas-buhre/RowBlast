#include "Scene.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "CameraComponent.hpp"

using namespace Pht;

Scene::Scene() :
    mRoot {std::make_unique<SceneObject>(nullptr)} {}

Scene::~Scene() {}

SceneObject& Scene::GetRoot() {
    assert(mRoot);
    return *mRoot;
}

const SceneObject& Scene::GetRoot() const {
    assert(mRoot);
    return *mRoot;
}

void Scene::SetCamera(std::unique_ptr<SceneObject> camera) {
    assert(camera);
    assert(camera->GetComponent<CameraComponent>());
    mCamera = std::move(camera);
}

SceneObject& Scene::GetCamera() {
    assert(mCamera);
    return *mCamera;
}

const SceneObject& Scene::GetCamera() const {
    assert(mCamera);
    return *mCamera;
}
