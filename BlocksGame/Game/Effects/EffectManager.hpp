#ifndef EffectManager_hpp
#define EffectManager_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "ExplosionParticleEffect.hpp"
#include "LaserParticleEffect.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class EffectManager {
    public:
        enum class State {
            OngoingEffects,
            OnlyParticlesActiveInLaser,
            Inactive
        };

        EffectManager(Pht::IEngine& engine, GameScene& scene);

        void Init();
        void StartExplosion(const Pht::Vec2& position);
        void StartBigExplosion(const Pht::Vec2& position);
        void StartLaser(const Pht::Vec2& position);
        void StartLevelBombExplosion(const Pht::Vec2& position);
        void Update(float dt);

        State GetState() const {
            return mState;
        }

    private:
        State mState {State::Inactive};
        ExplosionParticleEffect mExplosionEffect;
        ExplosionParticleEffect mBigExplosionEffect;
        std::vector<std::unique_ptr<LaserParticleEffect>> mLaserEffects;
        std::vector<std::unique_ptr<ExplosionParticleEffect>> mLevelBombExplosionEffects;
    };
}

#endif
