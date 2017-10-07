#ifndef ExplosionParticleEffect_hpp
#define ExplosionParticleEffect_hpp

// Engine includes.
#include "ParticleSystem.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class ExplosionParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };
        
        ExplosionParticleEffect(Pht::IEngine& engine, const GameScene& scene);
        
        void StartExplosion(const Pht::Vec2& position);
        State Update();
        
        const Pht::ParticleEffect& GetOuterEffect() const {
            return mOuterParticleEffect;
        }

        const Pht::ParticleEffect& GetInnerEffect() const {
            return mInnerParticleEffect;
        }
        
    private:
        void InitInnerEffect(Pht::IEngine& engine);
        void InitOuterEffect(Pht::IEngine& engine);
        
        const GameScene& mScene;
        Pht::ParticleEffect mOuterParticleEffect;
        Pht::ParticleEffect mInnerParticleEffect;
    };
}

#endif
