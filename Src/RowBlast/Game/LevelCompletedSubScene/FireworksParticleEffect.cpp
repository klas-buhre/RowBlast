#include "FireworksParticleEffect.hpp"

#include <array>

// Engine includes.
#include "IEngine.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numFireworks {14};
    constexpr auto fireworksDuration {4.0f};
    constexpr auto fireworksWaitDuration {0.3f};
    constexpr auto fireworkTimeToLive {1.5f};
    constexpr auto fireworkTimeToLiveRandomPart {1.0f};
    constexpr auto velocity {25.0f};
    constexpr auto velocityRandomPart {1.0f};
    
    const std::array<Pht::Vec4, 3> colors {
        Pht::Vec4{1.0f, 0.6f, 0.6f, 1.0f},
        Pht::Vec4{0.6f, 1.0f, 0.6f, 1.0f},
        Pht::Vec4{0.6f, 0.6f, 1.0f, 1.0f},
    };
    
    Pht::Vec3 ParticleVelocityFunction() {
        auto theta {Pht::ToRadians(Pht::NormalizedRand() * 360.0f)};
        auto phi {Pht::ToRadians(Pht::NormalizedRand() * 360.0f)};
        auto sinTheta {std::sin(theta)};
        auto magnitude {velocity + (Pht::NormalizedRand() - 0.5f) * velocityRandomPart};
        
        return {
            static_cast<float>(magnitude * sinTheta * std::cos(phi)),
            static_cast<float>(magnitude * sinTheta * std::sin(phi)),
            static_cast<float>(magnitude * std::cos(theta))
        };
    }
}

FireworksParticleEffect::FireworksParticleEffect(Pht::IEngine& engine) :
    mEngine {engine} {

    mFireworks.resize(numFireworks);
    
    for (auto& firework: mFireworks) {
        firework = std::make_unique<Firework>(engine);
    }
}

void FireworksParticleEffect::Init(Pht::SceneObject& parentObject, const Pht::Vec3& effectsVolume) {
    for (auto& firework: mFireworks) {
        Pht::Vec3 position {
            (Pht::NormalizedRand() - 0.5f) * effectsVolume.x,
            (Pht::NormalizedRand() - 0.5f) * effectsVolume.y + effectsVolume.y / 5.0f,
            (Pht::NormalizedRand() - 0.5f) * effectsVolume.z - 10.0f
        };
        
        auto waitTime {
            Pht::NormalizedRand() *
            (fireworksDuration - fireworkTimeToLive - fireworkTimeToLiveRandomPart)
        };
        
        auto color {colors[std::rand() % colors.size()]};

        firework->Init(parentObject, position, waitTime, color);
    }
}

void FireworksParticleEffect::Start() {
    mState = State::Ongoing;
    mElapsedTime = 0.0f;
}

FireworksParticleEffect::State FireworksParticleEffect::Update() {
    switch (mState) {
        case State::Ongoing: {
            auto dt {mEngine.GetLastFrameSeconds()};
            mElapsedTime += dt;
            if (mElapsedTime > fireworksWaitDuration) {
                for (auto& firework: mFireworks) {
                    firework->Update(dt);
                }
            }
            if (mElapsedTime > fireworksDuration + fireworksWaitDuration) {
                mState = State::Inactive;
            }
            break;
        }
        case State::Inactive:
            break;
    }
    
    return mState;
}

FireworksParticleEffect::Firework::Firework(Pht::IEngine& engine) :
    mEngine {engine} {

    Pht::ParticleSettings particleSettings {
        .mVelocityFunction = ParticleVelocityFunction,
        .mAcceleration = Pht::Vec3{0.0f, -11.0f, 0.0f},
        .mDragCoefficient = 3.0f,
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = fireworkTimeToLive,
        .mTimeToLiveRandomPart = fireworkTimeToLiveRandomPart,
        .mFadeOutDuration = 1.0f,
        .mSize = Pht::Vec2{0.7f, 0.7f},
        .mSizeRandomPart = 0.3f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.5f
    };

    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 140
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mExplosion = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    mExplosion->GetRenderable()->GetMaterial().SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
}

void FireworksParticleEffect::Firework::Init(Pht::SceneObject& parentObject,
                                             const Pht::Vec3& position,
                                             float waitTime,
                                             const Pht::Vec4& color) {
    parentObject.AddChild(*mExplosion);
    mExplosion->GetTransform().SetPosition(position);

    mWaitTime = waitTime;
    mState = State::Waiting;
    mElapsedTime = 0.0f;
    
    auto* particleEffect {mExplosion->GetComponent<Pht::ParticleEffect>()};
    assert(particleEffect);

    particleEffect->Stop();
    particleEffect->GetEmitter().GetParticleSettings().mColor = color;
}

void FireworksParticleEffect::Firework::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            mElapsedTime += dt;
            if (mElapsedTime > mWaitTime) {
                mState = State::Exploding;
                mExplosion->GetComponent<Pht::ParticleEffect>()->Start();
                PlayFireworksSound(mEngine);
            }
            break;
        case State::Exploding: {
            auto* explosionEffect {mExplosion->GetComponent<Pht::ParticleEffect>()};
            explosionEffect->Update(dt);
            if (!explosionEffect->IsActive()) {
                mState = State::Inactive;
            }
            break;
        }
        case State::Inactive:
            break;
    }
}
