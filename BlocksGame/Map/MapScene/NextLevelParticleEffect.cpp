#include "NextLevelParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"

using namespace BlocksGame;

NextLevelParticleEffect::NextLevelParticleEffect() {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = {0.0f, 0.7f, 0.0f},
        .mSize = {1.7f, 0.85f, 1.7f},
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mFrequency = 5.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 2.0f,
        .mTimeToLiveRandomPart = 0.4f,
        .mFadeOutDuration = 1.2f,
        .mZAngularVelocityRandomPart = 200.0f,
        .mSize = 1.0f,
        .mSizeRandomPart = 0.5f,
        .mShrinkDuration = 1.2f
    };
    
    mParticleEffect.mParticleSystem = std::make_unique<Pht::ParticleEffect>(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
}

void NextLevelParticleEffect::StartEffect(const Pht::Vec3& position) {
    mParticleEffect.mTransform = Pht::Mat4::Translate(position.x, position.y, position.z);
    mParticleEffect.mParticleSystem->Start();
}

void NextLevelParticleEffect::Update(float dt) {
    mParticleEffect.mParticleSystem->Update(dt);
}
