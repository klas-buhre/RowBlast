#ifndef Planets_hpp
#define Planets_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    enum class PlanetType {
        Ogma,
        Titawin,
        Moon,
        Rayeon,
        Wadow
    };

    struct PlanetConfig {
        Pht::Vec3 mPosition;
        Pht::Vec3 mOrientation;
        float mSize {1.0f};
        float mAngularVelocity {1.0f};
        PlanetType mType;
    };
    
    class Planets {
    public:
        Planets(Pht::IEngine& engine,
                Pht::Scene& scene,
                int layerIndex,
                const std::vector<PlanetConfig>& planetConfigs,
                const Pht::Vec3& lightDirection,
                float lightIntensity);
        
        void Update();
        
    private:
        struct Planet {
            float mAngularVelocity;
            Pht::SceneObject& mSceneObject;
        };
    
        Pht::IEngine& mEngine;
        std::vector<Planet> mPlanets;
    };
}

#endif
