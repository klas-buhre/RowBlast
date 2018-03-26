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
        
        void Init();
        void StartExplosion(const Pht::Vec2& position);
        void Update(float dt);
        State GetState() const;
        
    private:
        void InitInnerEffect(Pht::IEngine& engine);
        void InitShockWave(Pht::IEngine& engine);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mShockWave;
        std::unique_ptr<Pht::SceneObject> mInnerParticleEffect;
    };
}

#endif
