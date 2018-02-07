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
        .mSize = 9.0f,
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.3f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mInnerParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Triangles);
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
        .mPointSize = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mPointSizeRandomPart = engine.GetRenderer().GetAdjustedNumPixels(60),
        .mShrinkDuration = 0.3f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mOuterParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Points);
}

void ExplosionParticleEffect::Init() {
    auto& effectsContainer {mScene.GetEffectsContainer()};
    effectsContainer.AddChild(*mInnerParticleEffect);
    effectsContainer.AddChild(*mOuterParticleEffect);
}

void ExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    const auto cellSize {mScene.GetCellSize()};

    Pht::Vec3 positionInField {
        position.x * cellSize + cellSize / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        mScene.GetFieldPosition().z
    };
    
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mInnerParticleEffect->GetTransform().SetPosition(positionInField);
    
    mOuterParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mOuterParticleEffect->GetTransform().SetPosition(positionInField);
}

ExplosionParticleEffect::State ExplosionParticleEffect::Update(float dt) {
    auto* outerEffect {mOuterParticleEffect->GetComponent<Pht::ParticleEffect>()};
    outerEffect->Update(dt);
    
    auto* innerEffect {mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()};
    innerEffect->Update(dt);
    
    if (outerEffect->IsActive() || innerEffect->IsActive()) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
