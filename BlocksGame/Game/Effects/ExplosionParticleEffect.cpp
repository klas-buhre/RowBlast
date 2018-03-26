#include "ExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

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

ExplosionParticleEffect::ExplosionParticleEffect(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene} {

    InitInnerEffect(engine);
    InitShockWave(engine);
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
        .mTextureFilename = "flare24.png",
        .mTimeToLive = 0.25f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.15f,
        .mSize = 22.0f,
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mInnerParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Triangles);
    mInnerParticleEffect->GetRenderable()->GetMaterial().SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
}

void ExplosionParticleEffect::InitShockWave(Pht::IEngine& engine) {
/*
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
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 0.4f,
        .mTimeToLiveRandomPart = 0.2f,
        .mFadeOutDuration = 0.0f,
        .mPointSize = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mPointSizeRandomPart = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mShrinkDuration = 0.3f
    };
*/
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_halo.png",
        .mTimeToLive = 0.35f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.35f,
        .mSize = 13.5f,
        .mSizeRandomPart = 0.0f,
        .mInitialSize = 2.0f,
        .mGrowDuration = 0.35f,
        .mShrinkDuration = 0.0f
    };

    auto& particleSystem {engine.GetParticleSystem()};
    mShockWave = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
}

void ExplosionParticleEffect::Init() {
    mScene.GetFlyingBlocksContainer().AddChild(*mInnerParticleEffect);
    mScene.GetEffectsContainer().AddChild(*mShockWave);
}

void ExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    const auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};

    Pht::Vec3 positionInScene {
        position.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z + 10.0f
    };
    
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mInnerParticleEffect->GetTransform().SetPosition(positionInScene);

    Pht::Vec3 positionInField {
        position.x * cellSize + cellSize / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        mScene.GetFieldPosition().z
    };

    mShockWave->GetComponent<Pht::ParticleEffect>()->Start();
    mShockWave->GetTransform().SetPosition(positionInField);
}

void ExplosionParticleEffect::Update(float dt) {
    mShockWave->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
}

ExplosionParticleEffect::State ExplosionParticleEffect::GetState() const {
    auto* shockWave {mShockWave->GetComponent<Pht::ParticleEffect>()};
    auto* innerEffect {mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()};
    
    if (shockWave->IsActive() || innerEffect->IsActive()) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
