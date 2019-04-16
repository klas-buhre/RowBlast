#ifndef TutorialLaserParticleEffect_hpp
#define TutorialLaserParticleEffect_hpp

#include <memory>

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class TutorialLaserParticleEffect {
    public:
        TutorialLaserParticleEffect(Pht::IEngine& engine, Pht::SceneObject& parent);
        
        void SetUp();
        void StartLaser();
        void Update(float dt);
        
        Pht::SceneObject& GetSceneObject() {
            return *mContainer;
        }
        
    private:
        void CreateThickBeam(Pht::IEngine& engine);
        void CreateThinBeam(Pht::IEngine& engine);
        void CreateFlare(Pht::IEngine& engine);
        void CreateParticles(Pht::IEngine& engine);
        
        std::unique_ptr<Pht::SceneObject> mContainer;
        std::unique_ptr<Pht::SceneObject> mThickBeam;
        std::unique_ptr<Pht::SceneObject> mThinBeam;
        std::unique_ptr<Pht::SceneObject> mFlare;
        std::unique_ptr<Pht::SceneObject> mParticles;
    };
}

#endif
