#include "SceneResources.hpp"

using namespace Pht;

void SceneResources::AddSceneObject(std::unique_ptr<SceneObject> sceneObject) {
    mSceneObjects.push_back(std::move(sceneObject));
}

void SceneResources::AddRenderableObject(std::unique_ptr<RenderableObject> renderableObject) {
    mRenderableObjects.push_back(std::move(renderableObject));
}
