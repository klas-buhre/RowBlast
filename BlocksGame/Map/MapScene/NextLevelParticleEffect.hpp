#ifndef NextLevelParticleEffect_hpp
#define NextLevelParticleEffect_hpp

// Engine includes.
#include "ParticleEffect.hpp"
#include "Vector.hpp"

namespace BlocksGame {
    class GameScene;
    
    class NextLevelParticleEffect {
    public:
        NextLevelParticleEffect();
        
        void StartEffect(const Pht::Vec3& position);
        void Update(float dt);
        
        const Pht::ParticleEffect_& GetEffect() const {
            return mParticleEffect;
        }
        
    private:
        Pht::ParticleEffect_ mParticleEffect;
    };
}

#endif
