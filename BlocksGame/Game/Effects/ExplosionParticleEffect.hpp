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
        
        ExplosionParticleEffect(Pht::IEngine& engine, const GameScene& scene);
        
        void StartExplosion(const Pht::Vec2& position);
        State Update(float dt);
        
        const Pht::SceneObject& GetOuterEffect() const {
            return *mOuterParticleEffect;
        }

        const Pht::SceneObject& GetInnerEffect() const {
            return *mInnerParticleEffect;
        }
        
    private:
        void InitInnerEffect(Pht::IEngine& engine);
        void InitOuterEffect(Pht::IEngine& engine);
        
        const GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mOuterParticleEffect;
        std::unique_ptr<Pht::SceneObject> mInnerParticleEffect;
    };
}

#endif
