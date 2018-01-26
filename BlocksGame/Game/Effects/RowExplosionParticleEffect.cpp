#include "RowExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

RowExplosionParticleEffect::RowExplosionParticleEffect(Pht::IEngine& engine, GameScene& scene) :
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
    
    auto& particleSystem {engine.GetParticleSystem()};
    mScenObject = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Points);
}

void RowExplosionParticleEffect::Init() {
    mScene.GetEffectsContainer().AddChild(*mScenObject);
}

void RowExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    const auto cellSize {mScene.GetCellSize()};

    Pht::Vec3 positionInField {
        position.x * cellSize + cellSize / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        mScene.GetFieldPosition().z
    };
    
    mScenObject->SetPosition(positionInField);
    mScenObject->GetComponent<Pht::ParticleEffect>()->Start();
}

RowExplosionParticleEffect::State RowExplosionParticleEffect::Update(float dt) {
    auto* effect {mScenObject->GetComponent<Pht::ParticleEffect>()};
    effect->Update(dt);
    
    if (effect->IsActive()) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
