#include "ConfettiParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto velocity {35.0f};
    constexpr auto velocityRandomPart {50.0f};
    
    Pht::Vec3 LeftParticleVelocityFunction() {
        auto theta {Pht::ToRadians(55.0f + (Pht::NormalizedRand() - 0.5f) * 60.0f)};
        auto magnitude {velocity + (Pht::NormalizedRand() - 0.5f) * velocityRandomPart};
        
        return {
            static_cast<float>(magnitude * cos(theta)),
            static_cast<float>(magnitude * sin(theta)),
            0.0f
        };
    }

    Pht::Vec3 RightParticleVelocityFunction() {
        auto theta {Pht::ToRadians(125.0f + (Pht::NormalizedRand() - 0.5f) * 60.0f)};
        auto magnitude {velocity + (Pht::NormalizedRand() - 0.5f) * velocityRandomPart};
        
        return {
            static_cast<float>(magnitude * cos(theta)),
            static_cast<float>(magnitude * sin(theta)),
            0.0f
        };
    }

    std::unique_ptr<Pht::SceneObject> CreateConfettiEffect(Pht::IEngine& engine,
                                                           const Pht::Vec4& color,
                                                           std::function<Pht::Vec3()> velocityFunction,
                                                           const Pht::Vec3& gravity) {
        Pht::ParticleSettings particleSettings {
            .mVelocityFunction = velocityFunction,
            .mAcceleration = gravity,
            .mDragCoefficient = 2.2f,
            .mColor = color,
            .mColorRandomPart = Pht::Vec4{0.4f, 0.4f, 0.4f, 0.0f},
            .mTextureFilename = "confetti.png",
            .mTimeToLive = 5.0f,
            .mTimeToLiveRandomPart = 0.0f,
            .mFadeOutDuration = 0.0f,
            .mZAngularVelocity = 400.0f,
            .mZAngularVelocityRandomPart = 600.0f,
            .mSize = Pht::Vec2{0.39f, 0.15f},
            .mSizeRandomPart = 0.05f,
            .mGrowDuration = 0.0f,
            .mShrinkDuration = 0.0f
        };

        Pht::EmitterSettings particleEmitterSettings {
            .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
            .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
            .mTimeToLive = 0.0f,
            .mFrequency = 0.0f,
            .mBurst = 180
        };
        
        auto& particleSystem {engine.GetParticleSystem()};
        auto sceneObject {
            particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                           particleEmitterSettings,
                                                           Pht::RenderMode::Triangles)
        };
        
        auto& material {sceneObject->GetRenderable()->GetMaterial()};
        material.SetBlend(Pht::Blend::No);
        material.GetDepthState().mDepthWrite = false;
        return sceneObject;
    }
}

ConfettiParticleEffect::ConfettiParticleEffect(Pht::IEngine& engine) :
    mEngine {engine} {

    mLeftEffect = CreateConfettiEffect(engine,
                                       {1.0f, 0.3f, 0.7f, 1.0f},
                                       LeftParticleVelocityFunction,
                                       {-2.75f, -16.0f, 0.0f});
    mRightEffect = CreateConfettiEffect(engine,
                                        {0.5f, 0.5f, 1.0f, 1.0f},
                                        RightParticleVelocityFunction,
                                        {2.75f, -16.0f, 0.0f});
}

void ConfettiParticleEffect::Init(Pht::SceneObject& parentObject, const Pht::Vec3& effectsVolume) {
    mState = State::Inactive;
    
    Pht::Vec3 leftPosition {
        -effectsVolume.x / 2.0f,
        -effectsVolume.y / 2.0f,
        effectsVolume.z / 5.0f
    };
    
    parentObject.AddChild(*mLeftEffect);
    mLeftEffect->GetTransform().SetPosition(leftPosition);

    Pht::Vec3 rightPosition {
        effectsVolume.x / 2.0f,
        -effectsVolume.y / 2.0f,
        effectsVolume.z / 5.0f
    };
    
    parentObject.AddChild(*mRightEffect);
    mRightEffect->GetTransform().SetPosition(rightPosition);
    
    mLeftEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mRightEffect->GetComponent<Pht::ParticleEffect>()->Stop();
}

void ConfettiParticleEffect::Start() {
    mState = State::Ongoing;
    mLeftEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mRightEffect->GetComponent<Pht::ParticleEffect>()->Start();
}

ConfettiParticleEffect::State ConfettiParticleEffect::Update() {
    switch (mState) {
        case State::Ongoing: {
            auto dt {mEngine.GetLastFrameSeconds()};
            auto* leftEffect {mLeftEffect->GetComponent<Pht::ParticleEffect>()};
            leftEffect->Update(dt);
            auto* rightEffect {mRightEffect->GetComponent<Pht::ParticleEffect>()};
            rightEffect->Update(dt);
            if (!leftEffect->IsActive() && !rightEffect->IsActive()) {
                mState = State::Inactive;
            }
            break;
        }
        case State::Inactive:
            break;
    }
    
    return mState;
}
