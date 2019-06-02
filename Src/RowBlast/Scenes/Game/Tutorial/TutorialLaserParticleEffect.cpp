#include "TutorialLaserParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "SceneObject.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"
#include "TutorialUiLayer.hpp"

using namespace RowBlast;

namespace {
    Pht::Vec3 ParticleVelocity() {
        auto theta = Pht::NormalizedRand() * 2.0f * 3.1415f;
        return {0.0f, std::cos(theta), 0.0f};
    }
}

TutorialLaserParticleEffect::TutorialLaserParticleEffect(Pht::IEngine& engine,
                                                         Pht::SceneObject& parent) :
    mContainer {std::make_unique<Pht::SceneObject>()} {
    
    parent.AddChild(*mContainer);
    
    CreateThickBeam(engine);
    CreateThinBeam(engine);
    CreateFlare(engine);
    CreateParticles(engine);
}

void TutorialLaserParticleEffect::CreateThickBeam(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "laser_beam_red.png",
        .mTimeToLive = 0.2f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.15f,
        .mSize = Pht::Vec2{7.0f, 23.0f},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.15f,
        .mShrinkScale = Pht::Vec2{0.0f, 1.0f}
    };

    auto& particleSystem = engine.GetParticleSystem();
    mThickBeam = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    mThickBeam->GetRenderable()->GetMaterial().SetShaderId(Pht::ShaderId::ParticleTextureColor);
    mContainer->AddChild(*mThickBeam);
}

void TutorialLaserParticleEffect::CreateThinBeam(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "laser_beam_red.png",
        .mTimeToLive = 0.52f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.08f,
        .mSize = Pht::Vec2{7.0f, 6.0f},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.08f,
        .mShrinkScale = Pht::Vec2{0.0f, 1.0f}
    };

    auto& particleSystem = engine.GetParticleSystem();
    mThinBeam = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                               particleEmitterSettings,
                                                               Pht::RenderMode::Triangles);
    mThinBeam->GetRenderable()->GetMaterial().SetShaderId(Pht::ShaderId::ParticleTextureColor);
    mContainer->AddChild(*mThinBeam);
}

void TutorialLaserParticleEffect::CreateFlare(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = 0.25f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.15f,
        .mSize = Pht::Vec2{14.0f, 14.0f},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    mFlare = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                            particleEmitterSettings,
                                                            Pht::RenderMode::Triangles);
    mFlare->GetRenderable()->GetMaterial().SetShaderId(Pht::ShaderId::ParticleNoAlphaTexture);
    mContainer->AddChild(*mFlare);
}

void TutorialLaserParticleEffect::CreateParticles(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{7.0f, 1.0f, 0.0f},
        .mTimeToLive = 0.5f,
        .mFrequency = 30.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocityFunction = ParticleVelocity,
        .mColor = Pht::Vec4{1.0f, 0.5f, 0.8f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "particle_sprite_point_blurred.png",
        .mTimeToLive = 0.5f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.2f,
        .mSize = Pht::Vec2{1.0f, 1.0f},
        .mSizeRandomPart = 0.2f,
        .mGrowDuration = 0.05f,
        .mShrinkDuration = 0.2f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    mParticles = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    mContainer->AddChild(*mParticles);
}

void TutorialLaserParticleEffect::SetUp() {
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Stop();
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Stop();
    mFlare->GetComponent<Pht::ParticleEffect>()->Stop();
    mParticles->GetComponent<Pht::ParticleEffect>()->Stop();
    
    Pht::Vec3 beamCenterPosition {0.0f, -2.0f, TutorialUiLayer::bomb};
    mThickBeam->GetTransform().SetPosition(beamCenterPosition);
    mThinBeam->GetTransform().SetPosition(beamCenterPosition);
    mParticles->GetTransform().SetPosition(beamCenterPosition);

    Pht::Vec3 flarePosition {-2.0f, -2.0f, TutorialUiLayer::bomb};
    mFlare->GetTransform().SetPosition(flarePosition);
}

void TutorialLaserParticleEffect::StartLaser() {
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Start();
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Start();
    mParticles->GetComponent<Pht::ParticleEffect>()->Start();
    mFlare->GetComponent<Pht::ParticleEffect>()->Start();
}

void TutorialLaserParticleEffect::Update(float dt) {
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mFlare->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mParticles->GetComponent<Pht::ParticleEffect>()->Update(dt);
}
