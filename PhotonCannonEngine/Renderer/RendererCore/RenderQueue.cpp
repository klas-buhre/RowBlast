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
    
    bool IsDepthWrite(const SceneObject& sceneObject) {
        if (auto renderable {sceneObject.GetRenderable()}) {
            return renderable->GetMaterial().GetDepthState().mDepthWrite;
        }
        
        return false;
    }
    
    bool CompareEntries(const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
        if (a.mDepthWrite != b.mDepthWrite) {
            if (a.mDepthWrite) {
                // Render depth writing objects first.
                return true;
            }
            
            return false;
        }
        
        if (a.mDepthWrite && b.mDepthWrite) {
            // Both objects write depth so sort front to back.
            return a.mDistance < b.mDistance;
        }
        
        // Both objects do not write depth sort back to front.
        return a.mDistance > b.mDistance;
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

void RenderQueue::AddSceneObjects(const SceneObject& parentSceneObject) {
    if (parentSceneObject.IsVisible()) {
        mQueue[mSize] = Entry {0.0f, IsDepthWrite(parentSceneObject), &parentSceneObject};
        ++mSize;
        assert(mSize < mQueue.size());
    }
    
    for (auto& child: parentSceneObject.GetChildren()) {
        AddSceneObjects(*child);
    }
}

void RenderQueue::CalculateDistances(const Mat4& viewMatrix) {
    auto transposedViewMatrix {viewMatrix.Transposed()};
    
    for (auto i {0}; i < mSize; ++i) {
        auto& renderEntry {mQueue[i]};
        auto& sceneObjectPosition {renderEntry.mSceneObject->GetPosition()};
        
        switch (mDistanceFunction) {
            case DistanceFunction::CameraSpaceZ: {
                auto sceneObjectPosCamSpace {transposedViewMatrix * Vec4{sceneObjectPosition, 0.0f}};
                renderEntry.mDistance = sceneObjectPosCamSpace.z;
                break;
            }
            case DistanceFunction::WorldSpaceZ:
                renderEntry.mDistance = -sceneObjectPosition.z;
                break;
        }
    }
}
