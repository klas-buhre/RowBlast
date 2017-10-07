#include "ExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

namespace {
    const auto outerVelocity {10.0f};
    const auto outerVelocityRandomPart {5.0f};
    
    Pht::Vec3 OuterParticleVelocityFunction() {
        auto theta {Pht::NormalizedRand() * 360.0f};
        auto magnitude {outerVelocity + (Pht::NormalizedRand() - 0.5f) * outerVelocityRandomPart};
        
        return {
            static_cast<float>(magnitude * cos(theta)),
            static_cast<float>(magnitude * sin(theta)),
            0.0f
        };
    }
}

ExplosionParticleEffect::ExplosionParticleEffect(Pht::IEngine& engine, const GameScene& scene) :
    mScene {scene} {

    InitInnerEffect(engine);
    InitOuterEffect(engine);
}

void ExplosionParticleEffect::InitInnerEffect(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 0.5f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mSize = engine.GetRenderer().GetAdjustedNumPixels(420),
        .mSizeRandomPart = 0,
        .mShrinkDuration = 0.3f
    };
    
    mInnerParticleEffect.mParticleSystem =
        std::make_unique<Pht::ParticleSystem>(engine, particleSettings, particleEmitterSettings);
}

void ExplosionParticleEffect::InitOuterEffect(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 100
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocityFunction = OuterParticleVelocityFunction,
        .mAcceleration = Pht::Vec3{0.0f, -10.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 0.6f, 0.2f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 0.4f,
        .mTimeToLiveRandomPart = 0.2f,
        .mFadeOutDuration = 0.0f,
        .mSize = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mSizeRandomPart = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mShrinkDuration = 0.3f
    };
    
    mOuterParticleEffect.mParticleSystem =
        std::make_unique<Pht::ParticleSystem>(engine, particleSettings, particleEmitterSettings);
}

void ExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};

    auto translation {
        Pht::Mat4::Translate(position.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
                             position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
                             mScene.GetFieldPosition().z)
    };
    
    mInnerParticleEffect.mParticleSystem->Start();
    mInnerParticleEffect.mTransform = translation;
    
    mOuterParticleEffect.mParticleSystem->Start();
    mOuterParticleEffect.mTransform = translation;
}

ExplosionParticleEffect::State ExplosionParticleEffect::Update() {
    auto outerStatus {mOuterParticleEffect.mParticleSystem->Update()};
    auto innerStatus {mInnerParticleEffect.mParticleSystem->Update()};
    
    if (outerStatus || innerStatus) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
