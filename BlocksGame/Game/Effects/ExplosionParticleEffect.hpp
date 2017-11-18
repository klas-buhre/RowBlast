#ifndef ExplosionParticleEffect_hpp
#define ExplosionParticleEffect_hpp

// Engine includes.
#include "ParticleEffect.hpp"
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
        State Update(float dt);
        
        const Pht::ParticleEffect_& GetOuterEffect() const {
            return mOuterParticleEffect;
        }

        const Pht::ParticleEffect_& GetInnerEffect() const {
            return mInnerParticleEffect;
        }
        
    private:
        void InitInnerEffect();
        void InitOuterEffect(Pht::IEngine& engine);
        
        const GameScene& mScene;
        Pht::ParticleEffect_ mOuterParticleEffect;
        Pht::ParticleEffect_ mInnerParticleEffect;
    };
}

#endif
