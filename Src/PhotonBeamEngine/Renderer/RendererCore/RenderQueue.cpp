#include "RenderQueue.hpp"

#include <assert.h>
#include <algorithm>

#include "SceneObject.hpp"

using namespace Pht;

namespace {
    const Vec4 modelSpaceOrigin {0.0f, 0.0f, 0.0f, 1.0f};

    int CalcNumSceneObjects(const SceneObject& sceneObject) {
        auto numObjects = 1;
        
        for (auto& child: sceneObject.GetChildren()) {
            numObjects += CalcNumSceneObjects(*child);
        }
        
        return numObjects;
    }
    
    bool IsDepthWriting(const SceneObject& sceneObject) {
        if (auto renderable = sceneObject.GetRenderable()) {
            return renderable->GetMaterial().GetDepthState().mDepthWrite;
        }
        
        return false;
    }
    
    bool CompareEntriesOptimizedRendering(const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
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

    bool CompareEntriesBackToFront(const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
        return a.mDistance > b.mDistance;
    }
}

void RenderQueue::Init(const SceneObject& rootSceneObject) {
    mRootSceneObject = &rootSceneObject;
    auto numSceneObjects {CalcNumSceneObjects(rootSceneObject)};
    mQueue.resize(numSceneObjects);
}

void RenderQueue::Build(const Mat4& viewMatrix,
                        RenderOrder renderOrder,
                        DistanceFunction distanceFunction,
                        int layerMask) {
    mSize = 0;
    
    assert(mRootSceneObject);
    AddSceneObject(*mRootSceneObject, layerMask, false);
    CalculateDistances(viewMatrix, distanceFunction);
    
    switch (renderOrder) {
        case RenderOrder::Optimized:
            std::sort(&mQueue[0], &mQueue[mSize], CompareEntriesOptimizedRendering);
            break;
        case RenderOrder::BackToFront:
            std::sort(&mQueue[0], &mQueue[mSize], CompareEntriesBackToFront);
            break;
    }
}

void RenderQueue::AddSceneObject(const SceneObject& sceneObject,
                                 int layerMask,
                                 bool ancestorMatchedLayerMask) {
    if (!sceneObject.IsVisible()) {
        return;
    }
    
    auto thisObjectOrAncestorMatchedLayerMask = ancestorMatchedLayerMask;
    auto sceneObjectLayerMask = sceneObject.GetLayerMask();
    
    if (!ancestorMatchedLayerMask) {
        if (sceneObjectLayerMask & layerMask) {
            thisObjectOrAncestorMatchedLayerMask = true;
        }
    }
    
    if (sceneObjectLayerMask && !thisObjectOrAncestorMatchedLayerMask) {
        return;
    }
    
    if (thisObjectOrAncestorMatchedLayerMask) {
        mQueue[mSize] = Entry {0.0f, IsDepthWriting(sceneObject), &sceneObject};
        ++mSize;
        assert(mSize < mQueue.size());
    }
    
    for (auto& child: sceneObject.GetChildren()) {
        AddSceneObject(*child, layerMask, thisObjectOrAncestorMatchedLayerMask);
    }
}

void RenderQueue::CalculateDistances(const Mat4& viewMatrix, DistanceFunction distanceFunction) {
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto transposedViewMatrix = viewMatrix.Transposed();
    
    for (auto i {0}; i < mSize; ++i) {
        auto& renderEntry = mQueue[i];
        auto sceneObjectPos = renderEntry.mSceneObject->GetWorldSpacePosition();
        
        switch (distanceFunction) {
            case DistanceFunction::CameraSpaceZ: {
                auto sceneObjectPosCamSpace = transposedViewMatrix * Vec4{sceneObjectPos, 1.0f};
                renderEntry.mDistance = -sceneObjectPosCamSpace.z;
                break;
            }
            case DistanceFunction::WorldSpaceZ:
                renderEntry.mDistance = sceneObjectPos.z;
                break;
            case DistanceFunction::WorldSpaceNegativeZ: {
                renderEntry.mDistance = -sceneObjectPos.z;
                break;
            }
        }
    }
}
