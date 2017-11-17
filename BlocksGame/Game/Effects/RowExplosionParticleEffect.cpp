#include "RowExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

RowExplosionParticleEffect::RowExplosionParticleEffect(Pht::IEngine& engine,
                                                       const GameScene& scene) :
    mScene {scene} {

    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{30.0f, 0.2f, 0.2f},
        .mTimeToLive = 0.0f,
        .mBurst = 250
    };
    
    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.2f, 0.2f, 0.2f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 0.4f,
        .mTimeToLiveRandomPart = 0.2f,
        .mFadeOutDuration = 0.0f,
        .mPointSize = engine.GetRenderer().GetAdjustedNumPixels(100),
        .mPointSizeRandomPart = engine.GetRenderer().GetAdjustedNumPixels(100),
        .mShrinkDuration = 0.3f
    };

    mParticleEffect.mParticleSystem = std::make_unique<Pht::ParticleEffect>(engine,
                                                                            particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Points);
}

void RowExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};

    auto translation {
        Pht::Mat4::Translate(position.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
                             position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
                             mScene.GetFieldPosition().z)
    };
    
    mParticleEffect.mParticleSystem->Start();
    mParticleEffect.mTransform = translation;
}

RowExplosionParticleEffect::State RowExplosionParticleEffect::Update() {
    mParticleEffect.mParticleSystem->Update();
    
    if (mParticleEffect.mParticleSystem->IsActive()) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
