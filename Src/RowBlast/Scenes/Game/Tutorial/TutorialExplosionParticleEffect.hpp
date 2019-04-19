#ifndef TutorialExplosionParticleEffect_hpp
#define TutorialExplosionParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class ParticleSettings;
}

namespace RowBlast {
    class TutorialExplosionParticleEffect {
    public:
        enum class Kind {
            Bomb,
            LevelBomb
        };
        
        TutorialExplosionParticleEffect(Pht::IEngine& engine, Kind kind, Pht::SceneObject& parent);
        
        void SetUp();
        void StartExplosion(const Pht::Vec3& position);
        void Update(float dt);

        Pht::SceneObject& GetSceneObject() {
            return *mContainer;
        }

    private:
        void CreateInnerEffect(Pht::IEngine& engine, const Pht::ParticleSettings& particleSettings);
        void CreateShockWave(Pht::IEngine& engine, const Pht::ParticleSettings& particleSettings);
        
        std::unique_ptr<Pht::SceneObject> mContainer;
        std::unique_ptr<Pht::SceneObject> mShockWave;
        std::unique_ptr<Pht::SceneObject> mInnerParticleEffect;
    };
}

#endif
