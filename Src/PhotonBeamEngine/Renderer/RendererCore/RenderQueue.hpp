#ifndef RenderQueue_hpp
#define RenderQueue_hpp

#include <vector>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Scene.hpp"
#include "RenderPass.hpp"

namespace Pht {
    class SceneObject;
    
    class RenderQueue {
    public:
        void Init(const SceneObject& rootSceneObject);
        void Build(const Mat4& viewMatrix,
                   RenderOrder renderOrder,
                   DistanceFunction distanceFunction,
                   int layerMask);
        
        struct Entry {
            float mDistance;
            bool mDepthWrite;
            const SceneObject* mSceneObject;
        };

        // For range-based for loops.
        Entry* begin() {
            return &mQueue[0];
        }

        Entry* end() {
            return &mQueue[mSize];
        }

    private:
        void AddSceneObject(const SceneObject& sceneObject,
                            int layerMask,
                            bool ancestorMatchedLayerMask);
        void CalculateDistances(const Mat4& viewMatrix, DistanceFunction distanceFunction);
        
        const SceneObject* mRootSceneObject;
        std::vector<Entry> mQueue;
        int mSize {0};
    };
}

#endif
