#include "Scene.hpp"

#include <assert.h>

#include "SceneObject.hpp"

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
