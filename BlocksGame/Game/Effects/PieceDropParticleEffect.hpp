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
    
    using PieceDropParticleEffects = std::array<std::unique_ptr<Pht::SceneObject>, Field::maxNumColumns>;
    
    class PieceDropParticleEffect {
    public:
        PieceDropParticleEffect(Pht::IEngine& engine, const GameScene& scene);
        
        void StartEffect(const FallingPiece& fallingPiece);
        void Update(float dt);
        
        const PieceDropParticleEffects& GetEffects() const {
            return mParticleEffects;
        }
        
    private:
        void StartEffect(const Pht::Vec3& scenePosition, const Pht::Vec4& color);
        
        const GameScene& mScene;
        PieceDropParticleEffects mParticleEffects;
    };
}

#endif
