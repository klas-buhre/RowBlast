#ifndef AsteroidAnimation_hpp
#define AsteroidAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class SceneObject;
}

namespace RowBlast {
    class AsteroidAnimation {
    public:
        void Init();
        void Update(float dt);
        
        void SetSceneObject(Pht::SceneObject* asteroidSceneObject) {
            mAsteroidSceneObject = asteroidSceneObject;
        }
    
    private:
        Pht::SceneObject* mAsteroidSceneObject {nullptr};
        float mAnimationTime {0.0f};
    };
}

#endif
