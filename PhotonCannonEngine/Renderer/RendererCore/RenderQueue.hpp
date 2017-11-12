#ifndef RenderQueue_hpp
#define RenderQueue_hpp

#include <vector>

#include "Vector.hpp"

namespace Pht {
    class SceneObject;
    
    class RenderQueue {
    public:
        void AllocateQueue(const SceneObject& rootSceneObject);
        
    private:
        std::vector<const SceneObject*> mQueue;
    };
}

#endif
