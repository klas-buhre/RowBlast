#ifndef RenderQueue_hpp
#define RenderQueue_hpp

#include <vector>

#include "Vector.hpp"
#include "Matrix.hpp"

namespace Pht {
    class SceneObject;
    
    enum class DistanceFunction {
        CameraSpaceZ,
        WorldSpaceZ
    };
    
    class RenderQueue {
    public:
        RenderQueue(const SceneObject& rootSceneObject);
        
        void Allocate();
        void Build(const Mat4& viewMatrix);
        
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
        
        void SetDistanceFunction(DistanceFunction distanceFunction) {
            mDistanceFunction = distanceFunction;
        }

    private:
        void AddSceneObjects(const SceneObject& parentSceneObject);
        void CalculateDistances(const Mat4& viewMatrix);
        
        const SceneObject& mRootSceneObject;
        std::vector<Entry> mQueue;
        int mSize {0};
        DistanceFunction mDistanceFunction {DistanceFunction::CameraSpaceZ};
    };
}

#endif
