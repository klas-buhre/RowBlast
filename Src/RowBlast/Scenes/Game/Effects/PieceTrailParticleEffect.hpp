#ifndef PieceTrailParticleEffect_hpp
#define PieceTrailParticleEffect_hpp

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

namespace RowBlast {
    class GameScene;
    class FallingPiece;
    
    class PieceTrailParticleEffect {
    public:
        PieceTrailParticleEffect(Pht::IEngine& engine, GameScene& scene);
        
        void Init();
        void StartEffect(const FallingPiece& fallingPiece);
        void Update(float dt);
        
    private:
        using TrailEffects = std::array<std::unique_ptr<Pht::SceneObject>, 5>;
        
        void CreateTrailEffects(Pht::IEngine& engine,
                                const std::string& textureFilename,
                                TrailEffects& trailEffects);
        void StartTrailEffect(const Pht::Vec3& scenePosition, TrailEffects& trailEffects);
        void UpdateTrailEffects(float dt, TrailEffects& trailEffects);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        TrailEffects mBlueTrailEffects;
        TrailEffects mGreenTrailEffects;
        TrailEffects mRedTrailEffects;
        TrailEffects mYellowTrailEffects;
    };
}

#endif
