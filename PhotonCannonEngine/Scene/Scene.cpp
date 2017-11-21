#include "Scene.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
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

void Scene::SetLight(std::unique_ptr<SceneObject> light) {
    assert(light);
    
    mLight = light->GetComponent<LightComponent>();
    assert(mLight);

    mRoot->AddChild(std::move(light));
}

void Scene::SetCamera(std::unique_ptr<SceneObject> camera) {
    assert(camera);
    
    mCamera = camera->GetComponent<CameraComponent>();
    assert(mCamera);

    mRoot->AddChild(std::move(camera));
}

LightComponent& Scene::GetLight() {
    assert(mLight);
    return *mLight;
}

const LightComponent& Scene::GetLight() const {
    assert(mLight);
    return *mLight;
}

CameraComponent& Scene::GetCamera() {
    assert(mCamera);
    return *mCamera;
}

const CameraComponent& Scene::GetCamera() const {
    assert(mCamera);
    return *mCamera;
}

void Scene::SetDistanceFunction(DistanceFunction distanceFunction) {
    mRenderQueue.SetDistanceFunction(distanceFunction);
}
