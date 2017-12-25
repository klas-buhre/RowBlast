#ifndef NextLevelParticleEffect_hpp
#define NextLevelParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace BlocksGame {
    class GameScene;
    
    class NextLevelParticleEffect {
    public:
        NextLevelParticleEffect(Pht::IEngine& engine);
        
        void StartEffect(const Pht::Vec3& position);
        void Stop();
        
        const Pht::SceneObject& GetSceneObject() const {
            return *mScenObject;
        }
        
    private:
        std::unique_ptr<Pht::SceneObject> mScenObject;
    };
}

#endif
