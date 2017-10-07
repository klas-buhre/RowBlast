#ifndef RowExplosionParticleEffect_hpp
#define RowExplosionParticleEffect_hpp

// Engine includes.
#include "ParticleSystem.hpp"
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
        State Update();
        
        const Pht::ParticleEffect& GetEffect() const {
            return mParticleEffect;
        }
        
    private:
        const GameScene& mScene;
        Pht::ParticleEffect mParticleEffect;
    };
}

#endif
