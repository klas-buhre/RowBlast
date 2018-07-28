#include "ExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

ExplosionParticleEffect::ExplosionParticleEffect(Pht::IEngine& engine,
                                                 GameScene& scene,
                                                 Kind kind) :
    mScene {scene} {

    switch (kind) {
        case Kind::Bomb: {
            Pht::ParticleSettings innerParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "flare24.png",
                .mTimeToLive = 0.25f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.15f,
                .mSize = Pht::Vec2{22.0f, 22.0f},
                .mSizeRandomPart = 0.0f,
                .mGrowDuration = 0.0f,
                .mShrinkDuration = 0.0f
            };
            InitInnerEffect(engine, innerParticleSettings);
            Pht::ParticleSettings shockWaveParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "particle_sprite_halo.png",
                .mTimeToLive = 0.35f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.35f,
                .mSize = Pht::Vec2{13.5f, 13.5f},
                .mSizeRandomPart = 0.0f,
                .mInitialSize = Pht::Vec2{2.0f, 2.0f},
                .mGrowDuration = 0.35f,
                .mShrinkDuration = 0.0f
            };
            InitShockWave(engine, shockWaveParticleSettings);
            break;
        }
        case Kind::LevelBomb: {
            Pht::ParticleSettings innerParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "flare24.png",
                .mTimeToLive = 0.25f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.15f,
                .mSize = Pht::Vec2{16.5f, 16.5f},
                .mSizeRandomPart = 0.0f,
                .mGrowDuration = 0.0f,
                .mShrinkDuration = 0.0f
            };
            InitInnerEffect(engine, innerParticleSettings);
            Pht::ParticleSettings shockWaveParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "particle_sprite_halo.png",
                .mTimeToLive = 0.35f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.35f,
                .mSize = Pht::Vec2{10.0f, 10.0f},
                .mSizeRandomPart = 0.0f,
                .mInitialSize = Pht::Vec2{1.5f, 1.5f},
                .mGrowDuration = 0.35f,
                .mShrinkDuration = 0.0f
            };
            InitShockWave(engine, shockWaveParticleSettings);
            break;
        }
        case Kind::BigBomb: {
            Pht::ParticleSettings innerParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "flare24.png",
                .mTimeToLive = 0.25f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.15f,
                .mSize = Pht::Vec2{30.0f, 30.0f},
                .mSizeRandomPart = 0.0f,
                .mGrowDuration = 0.0f,
                .mShrinkDuration = 0.0f
            };
            InitInnerEffect(engine, innerParticleSettings);
            Pht::ParticleSettings shockWaveParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "particle_sprite_halo.png",
                .mTimeToLive = 0.42f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.42f,
                .mSize = Pht::Vec2{19.0f, 19.0f},
                .mSizeRandomPart = 0.0f,
                .mInitialSize = Pht::Vec2{2.8f, 2.8f},
                .mGrowDuration = 0.42f,
                .mShrinkDuration = 0.0f
            };
            InitShockWave(engine, shockWaveParticleSettings);
            break;
        }
    }
}

void ExplosionParticleEffect::InitInnerEffect(Pht::IEngine& engine,
                                              const Pht::ParticleSettings& particleSettings) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mInnerParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Triangles);
    auto& material {mInnerParticleEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
}

void ExplosionParticleEffect::InitShockWave(Pht::IEngine& engine,
                                            const Pht::ParticleSettings& particleSettings) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    auto& particleSystem {engine.GetParticleSystem()};
    mShockWave = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
}

void ExplosionParticleEffect::Init() {
    mScene.GetFlyingBlocksContainer().AddChild(*mInnerParticleEffect);
    mInnerParticleEffect->SetIsStatic(true);
    mScene.GetEffectsContainer().AddChild(*mShockWave);
    mShockWave->SetIsStatic(true);
}

void ExplosionParticleEffect::StartExplosion(const Pht::Vec2& position) {
    mInnerParticleEffect->SetIsStatic(false);
    mShockWave->SetIsStatic(false);

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
    auto* shockWave {mShockWave->GetComponent<Pht::ParticleEffect>()};
    auto* innerEffect {mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()};

    shockWave->Update(dt);
    innerEffect->Update(dt);
    
    if (!shockWave->IsActive()) {
        mShockWave->SetIsStatic(true);
    }
    
    if (!innerEffect->IsActive()) {
        mInnerParticleEffect->SetIsStatic(true);
    }
}

ExplosionParticleEffect::State ExplosionParticleEffect::GetState() const {
    auto* shockWave {mShockWave->GetComponent<Pht::ParticleEffect>()};
    auto* innerEffect {mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()};
    
    if (shockWave->IsActive() || innerEffect->IsActive()) {
        return State::Ongoing;
    }
    
    return State::Inactive;
}
