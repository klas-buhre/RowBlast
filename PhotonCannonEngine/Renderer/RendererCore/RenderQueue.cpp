#include "RenderQueue.hpp"

#include <assert.h>
#include <algorithm>

#include "SceneObject.hpp"

using namespace Pht;

namespace {
    const Vec4 modelSpaceOrigin {0.0f, 0.0f, 0.0f, 1.0f};

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

void RenderQueue::Init(const SceneObject& rootSceneObject) {
    mRootSceneObject = &rootSceneObject;
    auto numSceneObjects {CalcNumSceneObjects(rootSceneObject)};
    mQueue.resize(numSceneObjects);
}

void RenderQueue::Build(const Mat4& viewMatrix, DistanceFunction distanceFunction) {
    mSize = 0;
    
    assert(mRootSceneObject);
    AddSceneObjects(*mRootSceneObject);
    CalculateDistances(viewMatrix, distanceFunction);
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

void RenderQueue::CalculateDistances(const Mat4& viewMatrix, DistanceFunction distanceFunction) {
    auto transposedViewMatrix {viewMatrix.Transposed()};
    
    for (auto i {0}; i < mSize; ++i) {
        auto& renderEntry {mQueue[i]};
        auto sceneObjectPos {renderEntry.mSceneObject->GetWorldSpacePosition()};
        
        switch (distanceFunction) {
            case DistanceFunction::CameraSpaceZ: {
                auto sceneObjectPosCamSpace {transposedViewMatrix * Vec4{sceneObjectPos, 1.0f}};
                renderEntry.mDistance = -sceneObjectPosCamSpace.z;
                break;
            }
            case DistanceFunction::WorldSpaceZ: {
                renderEntry.mDistance = -sceneObjectPos.z;
                break;
            }
        }
    }
}
