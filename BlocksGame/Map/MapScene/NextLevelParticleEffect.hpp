#ifndef NextLevelParticleEffect_hpp
#define NextLevelParticleEffect_hpp

// Engine includes.
#include "ParticleSystem.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class NextLevelParticleEffect {
    public:
        NextLevelParticleEffect(Pht::IEngine& engine);
        
        void StartEffect(const Pht::Vec3& position);
        void Update();
        
        const Pht::ParticleEffect& GetEffect() const {
            return mParticleEffect;
        }
        
    private:
        Pht::ParticleEffect mParticleEffect;
    };
}

#endif
