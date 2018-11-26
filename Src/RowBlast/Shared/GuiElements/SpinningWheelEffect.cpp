#include "SpinningWheelEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSystem.hpp"

using namespace RowBlast;

SpinningWheelEffect::SpinningWheelEffect(Pht::IEngine& engine) :
    mEngine {engine} {

    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "spinning_wheel.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = -250.0f,
        .mSize = Pht::Vec2{3.6f, 3.6f},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.3f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mSceneObject = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                  particleEmitterSettings,
                                                                  Pht::RenderMode::Triangles);
}

void SpinningWheelEffect::Init(Pht::SceneObject& parentObject) {
    parentObject.AddChild(*mSceneObject);
    mSceneObject->GetComponent<Pht::ParticleEffect>()->Stop();
    mSceneObject->SetIsStatic(true);
}

void SpinningWheelEffect::Start() {
    mSceneObject->SetIsStatic(false);
    mSceneObject->GetComponent<Pht::ParticleEffect>()->Start();
}

void SpinningWheelEffect::Stop() {
    mSceneObject->GetComponent<Pht::ParticleEffect>()->Stop();
    mSceneObject->SetIsStatic(true);
}

void SpinningWheelEffect::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mSceneObject->GetComponent<Pht::ParticleEffect>()->Update(dt);
}
