#ifndef SunParticleEffect_hpp
#define SunParticleEffect_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace RowBlast {
    struct Sun {
        Pht::Vec3 mPosition;
        Pht::Vec2 mSize;
    };
    
    void CreateSunParticleEffect(Pht::IEngine& engine,
                                 Pht::Scene& scene,
                                 const Sun& sun,
                                 int layer);
}

#endif
