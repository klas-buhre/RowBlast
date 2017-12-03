#include "Scene.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "Fnv1Hash.hpp"
#include "ISceneManager.hpp"

using namespace Pht;

Scene::Scene(ISceneManager& sceneManager, Name name) :
    mSceneManager {sceneManager},
    mName {name} {
    
    mSceneObjects.push_back(std::make_unique<SceneObject>(Hash::Fnv1a("root")));
    mRoot = std::begin(mSceneObjects)->get();

    mRenderQueue = std::make_unique<RenderQueue>(*mRoot);
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

LightComponent& Scene::CreateGlobalLight() {
    auto sceneObject {std::make_unique<SceneObject>(Hash::Fnv1a("light"))};
    sceneObject->SetIsVisible(false);
    
    auto lightComponent {std::make_unique<LightComponent>(*sceneObject)};
    mGlobalLight = lightComponent.get();
    sceneObject->SetComponent<LightComponent>(std::move(lightComponent));
    
    AddSceneObject(std::move(sceneObject));
    return *mGlobalLight;
}

CameraComponent& Scene::CreateCamera() {
    auto sceneObject {std::make_unique<SceneObject>(Hash::Fnv1a("camera"))};
    sceneObject->SetIsVisible(false);
    
    auto cameraComponent {std::make_unique<CameraComponent>(*sceneObject)};
    mCamera = cameraComponent.get();
    sceneObject->SetComponent<CameraComponent>(std::move(cameraComponent));
    
    AddSceneObject(std::move(sceneObject));
    return *mCamera;
}

SceneObject& Scene::CreateSceneObject(const IMesh& mesh, const Material& material) {
    auto sceneObject {mSceneManager.CreateSceneObject(mesh, material)};
    
    auto& retVal {*sceneObject};
    AddSceneObject(std::move(sceneObject));
    return retVal;
}

TextComponent& Scene::CreateText(const std::string& text, const TextProperties& properties) {
    auto sceneObject {std::make_unique<SceneObject>()};
    
    auto textComponent {std::make_unique<TextComponent>(*sceneObject, text, properties)};
    
    auto& retVal {*textComponent};
    sceneObject->SetComponent<TextComponent>(std::move(textComponent));
    AddSceneObject(std::move(sceneObject));
    return retVal;
}

void Scene::AddSceneObject(std::unique_ptr<SceneObject> sceneObject) {
    mSceneObjects.push_back(std::move(sceneObject));
}

void Scene::SetDistanceFunction(DistanceFunction distanceFunction) {
    mRenderQueue->SetDistanceFunction(distanceFunction);
}
