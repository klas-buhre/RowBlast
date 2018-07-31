#ifndef LaserParticleEffect_hpp
#define LaserParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class GameScene;
    
    class LaserParticleEffect {
    public:
        enum class State {
            Ongoing,
            OnlyParticles,
            Inactive
        };
        
        LaserParticleEffect(Pht::IEngine& engine, GameScene& scene);
        
        void Init();
        void StartLaser(const Pht::Vec2& position);
        void Update(float dt);
        State GetState() const;
        
    private:
        void CreateThickBeam(Pht::IEngine& engine);
        void CreateThinBeam(Pht::IEngine& engine);
        void CreateFlare(Pht::IEngine& engine);
        void CreateParticles(Pht::IEngine& engine);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mThickBeam;
        std::unique_ptr<Pht::SceneObject> mThinBeam;
        std::unique_ptr<Pht::SceneObject> mFlare;
        std::unique_ptr<Pht::SceneObject> mParticles;
    };
}

#endif
