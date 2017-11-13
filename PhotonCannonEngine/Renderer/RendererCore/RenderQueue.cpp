#include "RenderQueue.hpp"

#include <assert.h>
#include <algorithm>

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
    
    bool CompareEntries(const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
        return a.mCameraSpaceZ > b.mCameraSpaceZ;
    }
}

RenderQueue::RenderQueue(const SceneObject& rootSceneObject) :
    mRootSceneObject {rootSceneObject} {}

void RenderQueue::Allocate() {
    auto numSceneObjects {CalcNumSceneObjects(mRootSceneObject)};
    mQueue.resize(numSceneObjects);
}

void RenderQueue::Build(const Mat4& viewMatrix) {
    mSize = 0;
    
    AddSceneObjects(mRootSceneObject);
    CalculateDistances(viewMatrix);
    std::sort(&mQueue[0], &mQueue[mSize], CompareEntries);
}

void RenderQueue::AddSceneObjects(const SceneObject& rootSceneObject) {
    if (rootSceneObject.IsVisible()) {
        mQueue[mSize] = Entry {0.0f, &rootSceneObject};
        ++mSize;
        assert(mSize < mQueue.size());
    }
    
    for (auto& child: rootSceneObject.GetChildren()) {
        AddSceneObjects(*child);
    }
}

void RenderQueue::CalculateDistances(const Mat4& viewMatrix) {
    auto transposedViewMatrix {viewMatrix.Transposed()};
    
    for (auto i {0}; i < mSize; ++i) {
        auto& renderEntry {mQueue[i]};
        auto& sceneObjectPosition {renderEntry.mSceneObject->GetPosition()};
        auto sceneObjectPosCamSpace {transposedViewMatrix * Vec4{sceneObjectPosition, 0.0f}};
        
        renderEntry.mCameraSpaceZ = sceneObjectPosCamSpace.z;
    }
}
