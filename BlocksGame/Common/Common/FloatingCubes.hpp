#ifndef FloatingCubes_hpp
#define FloatingCubes_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace BlocksGame {
    class CommonResources;

    struct Volume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
    };
    
    class FloatingCubes {
    public:
        FloatingCubes(Pht::IEngine& engine,
                      Pht::Scene* scene,
                      const std::vector<Volume>& volumes,
                      const CommonResources& commonResources,
                      float scale);
        
        void Reset();
        void Update();
        
        const Pht::SceneObject& GetSceneObject() const {
            return *mSceneObject;
        }
        
    private:
        static constexpr int numRenderables {4};

        struct FloatingCube {
            Pht::Vec3 mVelocity;
            Pht::Vec3 mAngularVelocity;
            std::unique_ptr<Pht::SceneObject> mSceneObject;
        };

        Pht::IEngine& mEngine;
        Pht::Scene* mScene {nullptr};
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        std::vector<FloatingCube> mCubes;
        std::array<std::shared_ptr<Pht::RenderableObject>, numRenderables> mCubeRenderables;
        std::vector<Volume> mVolumes;
    };
}

#endif
