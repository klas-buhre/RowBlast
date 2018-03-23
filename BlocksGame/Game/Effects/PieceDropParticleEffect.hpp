#ifndef PieceDropParticleEffect_hpp
#define PieceDropParticleEffect_hpp

#include <array>
#include <memory>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace BlocksGame {
    class GameScene;
    class FallingPiece;
    
    class PieceDropParticleEffect {
    public:
        PieceDropParticleEffect(Pht::IEngine& engine, GameScene& scene);
        
        void Init();
        void StartEffect(const FallingPiece& fallingPiece);
        void Update(float dt);
        
    private:
        void StartEffect(const Pht::Vec3& scenePosition);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        std::array<std::unique_ptr<Pht::SceneObject>, Field::maxNumColumns> mParticleEffects;
    };
}

#endif
