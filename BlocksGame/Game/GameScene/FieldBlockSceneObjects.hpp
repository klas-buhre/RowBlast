#ifndef FieldBlockSceneObjects_hpp
#define FieldBlockSceneObjects_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "SceneObject.hpp"

namespace BlocksGame {
    class Level;
    
    class FieldBlockSceneObjects {
    public:
        FieldBlockSceneObjects(Pht::SceneObject& parentSceneObject, const Level& level);
        
        void ReclaimAll();
        Pht::SceneObject& AccuireSceneObject();
        
    private:
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        std::vector<std::unique_ptr<Pht::SceneObject>> mSceneObjects;
        int mNextAvailableIndex {0};
    };
}

#endif
