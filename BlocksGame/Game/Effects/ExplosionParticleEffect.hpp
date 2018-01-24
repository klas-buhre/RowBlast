#ifndef ExplosionParticleEffect_hpp
#define ExplosionParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace BlocksGame {
    class GameScene;
    
    class ExplosionParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };
        
        ExplosionParticleEffect(Pht::IEngine& engine, GameScene& scene);
        
        void Reset();
        void StartExplosion(const Pht::Vec2& position);
        State Update(float dt);
        
    private:
        void InitInnerEffect(Pht::IEngine& engine);
        void InitOuterEffect(Pht::IEngine& engine);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mOuterParticleEffect;
        std::unique_ptr<Pht::SceneObject> mInnerParticleEffect;
    };
}

#endif
