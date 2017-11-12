#include "RenderQueue.hpp"

#include "SceneObject.hpp"

using namespace Pht;

namespace {
    int CalcNumSceneObjects(const SceneObject& sceneObject) {
        auto numObjects {1};
        
        for (auto& child: sceneObject.GetChildren()) {
            numObjects += CalcNumSceneObjects(*child);
        }
        
        return numObjects;
    }
}

void RenderQueue::AllocateQueue(const SceneObject& rootSceneObject) {
    auto numSceneObjects {CalcNumSceneObjects(rootSceneObject)};
    mQueue.resize(numSceneObjects);
}
