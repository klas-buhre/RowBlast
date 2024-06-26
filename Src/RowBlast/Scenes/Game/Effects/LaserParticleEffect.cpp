#include "LaserParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    Pht::Vec3 ParticleVelocity() {
        auto theta = Pht::NormalizedRand() * 2.0f * 3.1415f;
        return {0.0f, std::cos(theta), 0.0f};
    }
}

LaserParticleEffect::LaserParticleEffect(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene} {

    CreateThickBeam(engine);
    CreateThinBeam(engine);
    CreateFlare(engine);
    CreateParticles(engine);
}

void LaserParticleEffect::CreateThickBeam(Pht::IEngine& engine) {
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
        .mSize = Pht::Vec2{23.0f, 23.0f},
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
}

void LaserParticleEffect::CreateThinBeam(Pht::IEngine& engine) {
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
        .mSize = Pht::Vec2{22.0f, 6.0f},
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
}

void LaserParticleEffect::CreateFlare(Pht::IEngine& engine) {
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
}

void LaserParticleEffect::CreateParticles(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{22.0f, 1.0f, 0.0f},
        .mBurst = 50,
        .mTimeToLive = 0.4f,
        .mFrequency = 10.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocityFunction = ParticleVelocity,
        .mColor = Pht::Vec4{1.0f, 0.5f, 0.8f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "particle_sprite_point_blurred.png",
        .mTimeToLive = 0.5f,
        .mTimeToLiveRandomPart = 0.1f,
        .mFadeOutDuration = 0.2f,
        .mSize = Pht::Vec2{0.65f, 0.65f},
        .mSizeRandomPart = 0.3f,
        .mGrowDuration = 0.05f,
        .mShrinkDuration = 0.2f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    mParticles = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
}

void LaserParticleEffect::Init() {
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Stop();
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Stop();
    mFlare->GetComponent<Pht::ParticleEffect>()->Stop();
    mParticles->GetComponent<Pht::ParticleEffect>()->Stop();
    
    auto& effectsContainer = mScene.GetEffectsContainer();
    effectsContainer.AddChild(*mThinBeam);
    effectsContainer.AddChild(*mParticles);

    auto& container = mScene.GetFlyingBlocksContainer();
    container.AddChild(*mThickBeam);
    container.AddChild(*mFlare);
}

void LaserParticleEffect::StartLaser(const Pht::Vec2& position) {
    const auto cellSize = mScene.GetCellSize();
    auto& fieldLowerLeft = mScene.GetFieldLoweLeft();
    auto fieldWidth = mScene.GetFieldWidth();

    Pht::Vec3 beamCenterPositionInScene {
        fieldWidth / 2.0f + fieldLowerLeft.x,
        position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z + 10.0f
    };
    
    mThickBeam->GetTransform().SetPosition(beamCenterPositionInScene);
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Start();

    Pht::Vec3 beamCenterPositionInField {
        fieldWidth / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        mScene.GetFieldPosition().z + 1.0f
    };

    mThinBeam->GetTransform().SetPosition(beamCenterPositionInField);
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Start();

    mParticles->GetTransform().SetPosition(beamCenterPositionInField);
    mParticles->GetComponent<Pht::ParticleEffect>()->Start();

    Pht::Vec3 flarePositionInScene {
        position.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z + 10.0f
    };
    
    mFlare->GetTransform().SetPosition(flarePositionInScene);
    mFlare->GetComponent<Pht::ParticleEffect>()->Start();
}

void LaserParticleEffect::Update(float dt) {
    mThickBeam->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mThinBeam->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mFlare->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mParticles->GetComponent<Pht::ParticleEffect>()->Update(dt);
}

LaserParticleEffect::State LaserParticleEffect::GetState() const {
    if (mThickBeam->GetComponent<Pht::ParticleEffect>()->IsActive() ||
        mThinBeam->GetComponent<Pht::ParticleEffect>()->IsActive() ||
        mFlare->GetComponent<Pht::ParticleEffect>()->IsActive()) {
        
        return State::Ongoing;
    }
    
    if (mParticles->GetComponent<Pht::ParticleEffect>()->IsActive()) {
        return State::OnlyParticles;
    }
    
    return State::Inactive;
}
