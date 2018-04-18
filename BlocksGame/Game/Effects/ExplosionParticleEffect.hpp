#ifndef ExplosionParticleEffect_hpp
#define ExplosionParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class ParticleSettings;
}

namespace RowBlast {
    class GameScene;
    
    class ExplosionParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };
        
        enum class Kind {
            Bomb,
            LevelBomb,
            BigBomb
        };
        
        ExplosionParticleEffect(Pht::IEngine& engine, GameScene& scene, Kind kind);
        
        void Init();
        void StartExplosion(const Pht::Vec2& position);
        void Update(float dt);
        State GetState() const;
        
    private:
        void InitInnerEffect(Pht::IEngine& engine, const Pht::ParticleSettings& particleSettings);
        void InitShockWave(Pht::IEngine& engine, const Pht::ParticleSettings& particleSettings);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mShockWave;
        std::unique_ptr<Pht::SceneObject> mInnerParticleEffect;
    };
}

#endif
