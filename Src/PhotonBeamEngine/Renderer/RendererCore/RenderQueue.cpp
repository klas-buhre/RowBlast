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
        if (auto* renderable = sceneObject.GetRenderable()) {
            return renderable->GetMaterial().GetDepthState().mDepthWrite;
        }
        
        return false;
    }
    
/*
    struct CompareEntriesOptimizedRendering {
        inline bool operator() (const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
            if (a.IsDepthWrite() != b.IsDepthWrite()) {
                if (a.IsDepthWrite()) {
                    // Render depth writing objects first.
                    return true;
                }
                
                return false;
            }
            
            if (a.IsDepthWrite() && b.IsDepthWrite()) {
                // Both objects write depth so sort front to back.
                return a.mDistance < b.mDistance;
            }
            
            // Both objects do not write depth sort back to front.
            return a.mDistance > b.mDistance;
        }
    };
*/
    struct CompareEntriesFrontToBack {
        inline bool operator() (const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
            return a.mDistance < b.mDistance;
        }
    };
    
    struct CompareEntriesBackToFront {
        inline bool operator() (const RenderQueue::Entry& a, const RenderQueue::Entry& b) {
            return a.mDistance > b.mDistance;
        }
    };
}

void RenderQueue::Init(const SceneObject& rootSceneObject) {
    mRootSceneObject = &rootSceneObject;
    auto numSceneObjects = CalcNumSceneObjects(rootSceneObject);
    mQueue.resize(numSceneObjects);
    mMaxSize = numSceneObjects;
}

void RenderQueue::Build(const Mat4& viewMatrix,
                        RenderOrder renderOrder,
                        DistanceFunction distanceFunction,
                        int layerMask) {
    mFirstPartitionSize = 0;
    mSecondPartitionSize = 0;
    mRenderOrder = renderOrder;
    mLayerMask = layerMask;

    assert(mRootSceneObject);
    AddSceneObject(*mRootSceneObject, false);
    CalculateDistances(viewMatrix, distanceFunction);
    Sort();
    BeginIteration();
}

void RenderQueue::Sort() {
    switch (mRenderOrder) {
        case RenderOrder::StateOptimized:
        case RenderOrder::PiexelOptimized:
            SortFirstPartition<CompareEntriesFrontToBack>();
            SortSecondPartition<CompareEntriesBackToFront>();
            break;
        case RenderOrder::BackToFront:
            SortFirstPartition<CompareEntriesBackToFront>();
            break;
    }
}

template<typename Comparator>
void RenderQueue::SortFirstPartition() {
    std::sort(&mQueue[0], &mQueue[mFirstPartitionSize], Comparator{});
}

template<typename Comparator>
void RenderQueue::SortSecondPartition() {
    std::sort(&mQueue[mMaxSize - mSecondPartitionSize], &mQueue[mMaxSize], Comparator{});
}

void RenderQueue::AddSceneObject(const SceneObject& sceneObject, bool ancestorMatchedLayerMask) {
    if (!sceneObject.IsVisible()) {
        return;
    }
    
    auto thisObjectOrAncestorMatchedLayerMask = ancestorMatchedLayerMask;
    auto sceneObjectLayerMask = sceneObject.GetLayerMask();
    if (!ancestorMatchedLayerMask) {
        if (sceneObjectLayerMask & mLayerMask) {
            thisObjectOrAncestorMatchedLayerMask = true;
        }
    }
    
    if (sceneObjectLayerMask && !thisObjectOrAncestorMatchedLayerMask) {
        return;
    }
    
    if (thisObjectOrAncestorMatchedLayerMask) {
        auto isDepthWriting = IsDepthWriting(sceneObject);
        
        Entry entry {
            .mSortKey = EncodeSortKey(isDepthWriting),
            .mDistance = 0.0f,
            .mSceneObject = &sceneObject
        };
        
        assert(mFirstPartitionSize + mSecondPartitionSize < mMaxSize);
        
        switch (mRenderOrder) {
            case RenderOrder::StateOptimized:
            case RenderOrder::PiexelOptimized:
                if (isDepthWriting) {
                    mQueue[mFirstPartitionSize] = entry;
                    ++mFirstPartitionSize;
                } else {
                    mQueue[mMaxSize - mSecondPartitionSize - 1] = entry;
                    ++mSecondPartitionSize;
                }
                break;
            case RenderOrder::BackToFront:
                mQueue[mFirstPartitionSize] = entry;
                ++mFirstPartitionSize;
                break;
        }
    }
    
    for (auto& child: sceneObject.GetChildren()) {
        AddSceneObject(*child, thisObjectOrAncestorMatchedLayerMask);
    }
}

uint64_t RenderQueue::EncodeSortKey(bool isDepthWriting) {
    uint64_t key {0};
    if (isDepthWriting) {
        key |= depthWriteMask;
    }
    
    // Other IDs...
    
    return key;
}

void RenderQueue::CalculateDistances(const Mat4& viewMatrix, DistanceFunction distanceFunction) {
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto transposedViewMatrix = viewMatrix.Transposed();
    
    for (BeginIteration(); HasMoreEntries();) {
        auto& renderEntry = GetNextEntry();
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

void RenderQueue::BeginIteration() {
    if (mFirstPartitionSize > 0) {
        mIteratorIndex = 0;
        mCurrentPartitionEndIndex = mFirstPartitionSize;
    } else {
        mIteratorIndex = mMaxSize - mSecondPartitionSize;
        mCurrentPartitionEndIndex = mMaxSize;
    }
}

RenderQueue::Entry& RenderQueue::GetNextEntry() {
    assert(HasMoreEntries());
    auto& entry = mQueue[mIteratorIndex];
    
    ++mIteratorIndex;
    if (mIteratorIndex >= mCurrentPartitionEndIndex &&
        mCurrentPartitionEndIndex == mFirstPartitionSize) {
        
        mIteratorIndex = mMaxSize - mSecondPartitionSize;
        mCurrentPartitionEndIndex = mMaxSize;
    }
    
    return entry;
}
