#ifndef PortalParticleEffect_hpp
#define PortalParticleEffect_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace RowBlast {
    void CreatePortalParticleEffect(Pht::IEngine& engine,
                                    Pht::Scene& scene,
                                    const Pht::Vec3& position,
                                    int layer);
}

#endif
