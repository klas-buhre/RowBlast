#ifndef RenderQueue_hpp
#define RenderQueue_hpp

#include <vector>

#include "Vector.hpp"
#include "Matrix.hpp"

namespace Pht {
    class SceneObject;
    
    class RenderQueue {
    public:
        RenderQueue(const SceneObject& rootSceneObject);
        
        void Allocate();
        void Build(const Mat4& viewMatrix);
        
        struct Entry {
            float mCameraSpaceZ;
            const SceneObject* mSceneObject;
        };
        
        // For range-based for loops.
        const Entry* cbegin() const;
        const Entry* cend() const;
        
    private:
        void AddSceneObjects(const SceneObject& rootSceneObject);
        void CalculateDistances(const Mat4& viewMatrix);
        
        const SceneObject& mRootSceneObject;
        std::vector<Entry> mQueue;
        int mSize {0};
    };
}

#endif
