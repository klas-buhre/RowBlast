#ifndef FloatingCubes_hpp
#define FloatingCubes_hpp

#include <array>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class Field;
    class CommonResources;
    
    struct FloatingCube {
        Pht::Vec3 mPosition;
        Pht::Vec3 mVelocity;
        Pht::Vec3 mOrientation;
        Pht::Vec3 mAngularVelocity;
        const Pht::RenderableObject* mRenderable {nullptr};
    };
    
    struct Volume {
        Pht::Vec3 mPosition;
        Pht::Vec3 mSize;
    };
    
    class FloatingCubes {
    public:
        FloatingCubes(const std::vector<Volume>& volumes,
                      Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      float scale);
        
        void Reset();
        void Update();
        
        const std::vector<FloatingCube>& GetCubes() const {
            return mCubes;
        }
        
    private:
        static constexpr int numRenderables {4};
        
        Pht::IEngine& mEngine;
        std::vector<FloatingCube> mCubes;
        std::array<std::unique_ptr<Pht::RenderableObject>, numRenderables> mCubeRenderables;
        std::vector<Volume> mVolumes;
    };
}

#endif
