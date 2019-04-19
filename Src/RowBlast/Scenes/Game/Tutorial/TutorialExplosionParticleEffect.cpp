#include "TutorialExplosionParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

using namespace RowBlast;

TutorialExplosionParticleEffect::TutorialExplosionParticleEffect(Pht::IEngine& engine,
                                                                 Kind kind,
                                                                 Pht::SceneObject& parent) :
    mContainer {std::make_unique<Pht::SceneObject>()} {

    switch (kind) {
        case Kind::Bomb: {
            Pht::ParticleSettings innerParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "flare24.png",
                .mTimeToLive = 0.25f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.15f,
                .mSize = Pht::Vec2{17.6f, 17.6f},
                .mSizeRandomPart = 0.0f,
                .mGrowDuration = 0.0f,
                .mShrinkDuration = 0.0f
            };
            CreateInnerEffect(engine, innerParticleSettings);
            Pht::ParticleSettings shockWaveParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "particle_sprite_halo.png",
                .mTimeToLive = 0.35f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.35f,
                .mSize = Pht::Vec2{10.8f, 10.8f},
                .mSizeRandomPart = 0.0f,
                .mInitialSize = Pht::Vec2{2.0f, 2.0f},
                .mGrowDuration = 0.35f,
                .mShrinkDuration = 0.0f
            };
            CreateShockWave(engine, shockWaveParticleSettings);
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
                .mSize = Pht::Vec2{13.2f, 13.2f},
                .mSizeRandomPart = 0.0f,
                .mGrowDuration = 0.0f,
                .mShrinkDuration = 0.0f
            };
            CreateInnerEffect(engine, innerParticleSettings);
            Pht::ParticleSettings shockWaveParticleSettings {
                .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
                .mTextureFilename = "particle_sprite_halo.png",
                .mTimeToLive = 0.35f,
                .mTimeToLiveRandomPart = 0.0f,
                .mFadeOutDuration = 0.35f,
                .mSize = Pht::Vec2{8.0f, 8.0f},
                .mSizeRandomPart = 0.0f,
                .mInitialSize = Pht::Vec2{1.5f, 1.5f},
                .mGrowDuration = 0.35f,
                .mShrinkDuration = 0.0f
            };
            CreateShockWave(engine, shockWaveParticleSettings);
            break;
        }
    }
    
    parent.AddChild(*mContainer);
}

void TutorialExplosionParticleEffect::CreateInnerEffect(Pht::IEngine& engine,
                                                        const Pht::ParticleSettings& particleSettings) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    mInnerParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Triangles);
    auto& material = mInnerParticleEffect->GetRenderable()->GetMaterial();
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    mContainer->AddChild(*mInnerParticleEffect);
}

void TutorialExplosionParticleEffect::CreateShockWave(Pht::IEngine& engine,
                                                      const Pht::ParticleSettings& particleSettings) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    auto& particleSystem = engine.GetParticleSystem();
    mShockWave = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    mContainer->AddChild(*mShockWave);
}

void TutorialExplosionParticleEffect::SetUp() {
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mShockWave->GetComponent<Pht::ParticleEffect>()->Stop();
}

void TutorialExplosionParticleEffect::StartExplosion(const Pht::Vec3& position) {
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mInnerParticleEffect->GetTransform().SetPosition(position);

    mShockWave->GetComponent<Pht::ParticleEffect>()->Start();
    mShockWave->GetTransform().SetPosition(position);
}

void TutorialExplosionParticleEffect::Update(float dt) {
    mShockWave->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mInnerParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
}
