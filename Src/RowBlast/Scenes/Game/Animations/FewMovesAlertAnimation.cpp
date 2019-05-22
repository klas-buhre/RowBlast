#include "FewMovesAlertAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObjectUtils.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fewMovesAlertLimit = 4;
    constexpr auto animationDuration = 0.75f;
    constexpr auto animationScaleAmplitude = 0.06f;
    constexpr auto animationScaleAdd = 0.06f;
    constexpr auto animationTextScaleAmplitude = 0.41f;
    constexpr auto animationTextScaleAdd = 0.41f;
    constexpr auto particleEffectDuration = 0.35f;
}

FewMovesAlertAnimation::FewMovesAlertAnimation(Pht::IEngine& engine,
                                               GameScene& scene,
                                               const GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {
    
    CreateParticleEffect(engine);
}

void FewMovesAlertAnimation::CreateParticleEffect(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 0.75f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_halo.png",
        .mTimeToLive = particleEffectDuration,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = particleEffectDuration,
        .mSize = Pht::Vec2{2.0f, 2.0f},
        .mSizeRandomPart = 0.0f,
        .mInitialSize = Pht::Vec2{0.5f, 0.5f},
        .mGrowDuration = particleEffectDuration,
        .mShrinkDuration = 0.0f
    };

    auto& particleSystem = engine.GetParticleSystem();
    mParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                     particleEmitterSettings,
                                                                     Pht::RenderMode::Triangles);
}

void FewMovesAlertAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
    
    auto& hud = mScene.GetHud();
    mMovesRoundedCylinderContainer = &hud.GetMovesRoundedCylinderContainer();
    mMovesTextContainerSceneObject = &hud.GetMovesTextContainer();
    mMovesTextSceneObject = &hud.GetMovesTextSceneObject();
    
    hud.ShowBlueMovesIcon();
    
    mParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    hud.GetMovesTextContainer().AddChild(*mParticleEffect);
    mParticleEffect->GetTransform().SetPosition({0.05f, 0.0f, UiLayer::root});
}

void FewMovesAlertAnimation::Update(float dt) {
    switch (mState) {
        case State::Inactive:
            UpdateInInactiveState();
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::ZeroMoves:
            UpdateInZeroMovesState();
            break;
    }
}

void FewMovesAlertAnimation::UpdateInInactiveState() {
    if (mGameLogic.GetMovesLeft() <= fewMovesAlertLimit) {
        mState = State::Active;
        mElapsedTime = 0.0f;
        mScene.GetHud().ShowYellowMovesIcon();
        mParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }
}

void FewMovesAlertAnimation::UpdateInActiveState(float dt) {
    auto movesLeft = mGameLogic.GetMovesLeft();
    if (movesLeft > fewMovesAlertLimit) {
        mState = State::Inactive;
        RestoreHud();
    } else if (movesLeft <= 0) {
        mState = State::ZeroMoves;
        RestoreHud();
    } else {
        mElapsedTime += dt;
        if (mElapsedTime > animationDuration) {
            mElapsedTime = 0.0f;
            mParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
        }
        
        auto t = mElapsedTime * 2.0f * 3.1415f / animationDuration;
        auto sinT = std::sin(t);

        auto movesContainerScale = 1.0f + animationScaleAdd + animationScaleAmplitude * sinT;
        mMovesRoundedCylinderContainer->GetTransform().SetScale(movesContainerScale);
        
        auto textContainerScale = 1.0f + animationTextScaleAdd + animationTextScaleAmplitude * sinT;
        mMovesTextContainerSceneObject->GetTransform().SetScale(textContainerScale);
        
        auto textScale =
            GameHud::movesContainerScale + animationTextScaleAdd +
            animationTextScaleAmplitude * sinT;
        auto fontSizeAdjustedTextScale = textScale * mScene.GetHud().GetMovesTextScaleFactor();
        Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextSceneObject, fontSizeAdjustedTextScale);
        
        mParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}

void FewMovesAlertAnimation::UpdateInZeroMovesState() {
    if (mGameLogic.GetMovesLeft() > fewMovesAlertLimit) {
        mState = State::Inactive;
        RestoreHud();
    }
}

void FewMovesAlertAnimation::RestoreHud() {
    mScene.GetHud().ShowBlueMovesIcon();
    mMovesRoundedCylinderContainer->GetTransform().SetScale(1.0f);
    mMovesTextContainerSceneObject->GetTransform().SetScale(GameHud::movesTextStaticScale);
    
    auto textScale =
        GameHud::movesContainerScale * GameHud::movesTextStaticScale *
        mScene.GetHud().GetMovesTextScaleFactor();
    Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextSceneObject, textScale);
    
    mParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
}
