#ifndef RowExplosionParticleEffect_hpp
#define RowExplosionParticleEffect_hpp

// Engine includes.
#include "ParticleEffect.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class RowExplosionParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };
        
        RowExplosionParticleEffect(Pht::IEngine& engine, const GameScene& scene);
        
        void StartExplosion(const Pht::Vec2& position);
        State Update(float dt);
        
        const Pht::ParticleEffect_& GetEffect() const {
            return mParticleEffect;
        }
        
    private:
        const GameScene& mScene;
        Pht::ParticleEffect_ mParticleEffect;
    };
}

#endif
