#ifndef FloatingCubes_hpp
#define FloatingCubes_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace BlocksGame {
    class CommonResources;

    struct CubePathVolume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
    };
    
    class FloatingCubes {
    public:
        FloatingCubes(Pht::IEngine& engine,
                      Pht::Scene& scene,
                      int layerIndex,
                      const std::vector<CubePathVolume>& volumes,
                      const CommonResources& commonResources,
                      float scale);
        
        void Update();
        
    private:
        void InitCubes();
        
        static constexpr int numRenderables {4};

        struct FloatingCube {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            std::unique_ptr<Pht::SceneObject> mSceneObject;
        };

        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        std::vector<FloatingCube> mCubes;
        std::array<std::unique_ptr<Pht::RenderableObject>, numRenderables> mCubeRenderables;
        std::vector<CubePathVolume> mVolumes;
    };
}

#endif
