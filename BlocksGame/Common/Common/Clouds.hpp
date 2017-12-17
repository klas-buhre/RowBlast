#ifndef Clouds_hpp
#define Clouds_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace BlocksGame {
    struct CloudPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
        Pht::Vec2 mCloudSize {40.0f, 40.0f};
    };
    
    class Clouds {
    public:
        Clouds(Pht::IEngine& engine,
               Pht::Scene& scene,
               int layerIndex,
               const std::vector<CloudPathVolume>& volumes,
               float velocity);
        
        void Update();
        
    private:
        struct Cloud {
            Pht::Vec3 mVelocity;
            Pht::SceneObject* mSceneObject;
        };

        Pht::IEngine& mEngine;
        std::vector<Cloud> mClouds;
        std::vector<CloudPathVolume> mPathVolumes;
    };
}

#endif
