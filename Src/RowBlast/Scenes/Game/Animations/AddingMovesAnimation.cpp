#include "AddingMovesAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"
#include "AudioResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto addMoveWaitDuration = 0.265f;
    constexpr auto numMovesToAdd = 5;
}

AddingMovesAnimation::AddingMovesAnimation(Pht::IEngine& engine, GameScene& scene) :
    mAudio {engine.GetAudio()},
    mScene {scene} {
    
    CreateParticleEffect(engine);
}

void AddingMovesAnimation::CreateParticleEffect(Pht::IEngine& engine) {
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
        .mTextureFilename = "particle_sprite_halo.png",
        .mTimeToLive = addMoveWaitDuration,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = addMoveWaitDuration,
        .mSize = Pht::Vec2{4.0f, 4.0f},
        .mSizeRandomPart = 0.0f,
        .mInitialSize = Pht::Vec2{0.65f, 0.65f},
        .mGrowDuration = addMoveWaitDuration,
        .mShrinkDuration = 0.0f
    };

    auto& particleSystem = engine.GetParticleSystem();
    mParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                     particleEmitterSettings,
                                                                     Pht::RenderMode::Triangles);
}

void AddingMovesAnimation::Init() {
    GoToInactiveState();
    mParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mScene.GetHud().GetMovesTextContainer().AddChild(*mParticleEffect);
    mParticleEffect->GetTransform().SetPosition({0.05f, 0.0f, UiLayer::root});
}

void AddingMovesAnimation::Start() {
    mState = State::Active;
    mElapsedTime = 0.0f;
    mMovesLeft = 0;
    
    auto& hud = mScene.GetHud();
    hud.SetNumMovesLeftSource(GameHud::NumMovesLeftSource::Self);
    hud.SetNumMovesLeft(0);
}

void AddingMovesAnimation::Update(float dt) {
    switch (mState) {
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void AddingMovesAnimation::UpdateInActiveState(float dt) {
    auto* particleEffect = mParticleEffect->GetComponent<Pht::ParticleEffect>();
    
    mElapsedTime += dt;
    if (mElapsedTime >= addMoveWaitDuration) {
        mElapsedTime = 0.0f;
        mMovesLeft++;
        
        if (mMovesLeft > numMovesToAdd) {
            GoToInactiveState();
        } else {
            mScene.GetHud().SetNumMovesLeft(mMovesLeft);
            particleEffect->Start();
            mAudio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::AddMove));
        }
    }
    
    particleEffect->Update(dt);
}

void AddingMovesAnimation::GoToInactiveState() {
    mState = State::Inactive;
    mParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mScene.GetHud().SetNumMovesLeftSource(GameHud::NumMovesLeftSource::GameLogic);
}
