#ifndef Clouds_hpp
#define Clouds_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    struct CloudPathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
        Pht::Vec2 mCloudSize {40.0f, 40.0f};
        float mCloudSizeRandPart {0.0f};
        int mNumClouds {1};
        Pht::Optional<int> mNumCloudsPerCluster;
        Pht::Vec2 mClusterSize {40.0f, 30.0f};
        Pht::Optional<float> mVelocity;
    };
    
    struct HazeLayer {
        Pht::Vec3 mPosition;
        Pht::Vec2 mSize;
        Pht::Vec4 mUpperColor;
        Pht::Vec4 mLowerColor;
    };
    
    class Clouds {
    public:
        Clouds(Pht::IEngine& engine,
               Pht::Scene& scene,
               int layerIndex,
               const std::vector<CloudPathVolume>& volumes,
               const std::vector<HazeLayer>& hazeLayers,
               float velocity);
        
        void Update();
        
    private:
        void InitHazeLayers(const std::vector<HazeLayer>& hazeLayers,
                            Pht::Scene& scene,
                            int sceneLayerIndex);
        
        struct Cloud {
            Pht::Vec3 mVelocity;
            Pht::SceneObject& mSceneObject;
            CloudPathVolume& mPathVolume;
        };

        Pht::IEngine& mEngine;
        std::vector<Cloud> mClouds;
        std::vector<CloudPathVolume> mPathVolumes;
    };
}

#endif
