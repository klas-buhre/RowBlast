#ifndef NextLevelParticleEffectNew_hpp
#define NextLevelParticleEffectNew_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace BlocksGame {
    void CreateNextLevelParticleEffect(Pht::IEngine& engine,
                                       Pht::Scene& scene,
                                       const Pht::Vec3& position,
                                       int layer);
}

#endif