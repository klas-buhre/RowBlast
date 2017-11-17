#ifndef PieceDropParticleEffect_hpp
#define PieceDropParticleEffect_hpp

#include <array>

// Engine includes.
#include "ParticleEffect.hpp"
#include "Vector.hpp"

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    class FallingPiece;
    
    using PieceDropParticleEffects = std::array<Pht::ParticleEffect_, Field::maxNumColumns>;
    
    class PieceDropParticleEffect {
    public:
        PieceDropParticleEffect(Pht::IEngine& engine, const GameScene& scene);
        
        void StartEffect(const FallingPiece& fallingPiece);
        void Update();
        
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
