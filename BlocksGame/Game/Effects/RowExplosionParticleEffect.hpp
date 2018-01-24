#ifndef RowExplosionParticleEffect_hpp
#define RowExplosionParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace BlocksGame {
    class GameScene;
    
    class RowExplosionParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };
        
        RowExplosionParticleEffect(Pht::IEngine& engine, GameScene& scene);
        
        void Reset();
        void StartExplosion(const Pht::Vec2& position);
        State Update(float dt);
        
    private:
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mScenObject;
    };
}

#endif
