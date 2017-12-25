#include "NextLevelParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "SceneObject.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSystem.hpp"

using namespace BlocksGame;

NextLevelParticleEffect::NextLevelParticleEffect(Pht::IEngine& engine) {
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
    
    auto& particleSystem {engine.GetParticleSystem()};
    mScenObject = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    particleSystem.AddParticleEffect(*mScenObject->GetComponent<Pht::ParticleEffect>());
}

void NextLevelParticleEffect::StartEffect(const Pht::Vec3& position) {
    mScenObject->SetPosition(position);
    mScenObject->GetComponent<Pht::ParticleEffect>()->Start();
}

void NextLevelParticleEffect::Stop() {
    mScenObject->GetComponent<Pht::ParticleEffect>()->Stop();
}
