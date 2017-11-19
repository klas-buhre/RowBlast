#include "Scene.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "CameraComponent.hpp"
#include "Fnv1Hash.hpp"

using namespace Pht;

Scene::Scene(Name name) :
    mName {name},
    mRoot {std::make_unique<SceneObject>(Hash::Fnv1a("root"))},
    mRenderQueue {*mRoot} {
    
    mRoot->SetIsVisible(false);
}

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

void Scene::SetDistanceFunction(DistanceFunction distanceFunction) {
    mRenderQueue.SetDistanceFunction(distanceFunction);
}
