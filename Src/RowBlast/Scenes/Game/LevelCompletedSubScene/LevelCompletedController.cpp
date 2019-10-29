#include "LevelCompletedController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "Scene.hpp"
#include "CameraComponent.hpp"
#include "IAudio.hpp"

// Game includes.
#include "GameViewControllers.hpp"
#include "SlidingText.hpp"
#include "SmallText.hpp"
#include "GameLogic.hpp"
#include "UserServices.hpp"
#include "Level.hpp"
#include "GameScene.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade = 0.5f;
    constexpr auto fadeTime = 0.3f;
    constexpr auto effectsVolumeDepth = 20.0f;
    constexpr auto fireworksDuration = 2.0f;
    constexpr auto smallTextAnimationWaintTime = 1.35f;
    constexpr auto waitTime = 0.95f;
    constexpr auto musicFadeOutTime = 1.2f;
    constexpr auto confettiWaitTime = 0.85f;

    const Pht::Vec3 initialUfoPosition {0.0f, 10.0f, 20.0f};
    const Pht::Vec3 ufoPosition {0.0f, 4.05f, 5.0f};
}

LevelCompletedController::LevelCompletedController(Pht::IEngine& engine,
                                                   GameScene& gameScene,
                                                   GameViewControllers& gameViewControllers,
                                                   SlidingText& slidingTextAnimation,
                                                   SmallText& smallTextAnimation,
                                                   GameLogic& gameLogic,
                                                   UserServices& userServices,
                                                   const CommonResources& commonResources,
                                                   Pht::CameraShake& cameraShake,
                                                   Field& field,
                                                   FlyingBlocksAnimation& flyingBlocksAnimation) :
    mEngine {engine},
    mGameScene {gameScene},
    mGameViewControllers {gameViewControllers},
    mSlidingText {slidingTextAnimation},
    mSmallText {smallTextAnimation},
    mGameLogic {gameLogic},
    mUserServices {userServices},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        -effectsVolumeDepth / 2.0f
    },
    mClearLastBlocksAnimation {field, flyingBlocksAnimation},
    mSlidingFieldAnimation {engine, gameScene},
    mFireworksParticleEffect {engine},
    mConfettiParticleEffect {engine},
    mStarsAnimation {engine, gameScene, commonResources, cameraShake},
    mUfo {engine, commonResources, 3.7f},
    mUfoAnimation {engine, mUfo} {
    
    mFadeEffect.GetSceneObject().SetLayer(static_cast<int>(GameScene::Layer::LevelCompletedFadeEffect));
}

void LevelCompletedController::Init(const Level& level) {
    mLevel = &level;
    
    auto& orthographicFrustumSize = mEngine.GetRenderer().GetOrthographicFrustumSize();

    Pht::Vec3 effectsVolume {
        orthographicFrustumSize.x, orthographicFrustumSize.y, effectsVolumeDepth
    };
    
    auto& container = mGameScene.GetLevelCompletedEffectsContainer();
    container.SetIsStatic(true);
    container.SetIsVisible(false);
    
    mFireworksParticleEffect.Init(container, effectsVolume);
    mConfettiParticleEffect.Init(container, effectsVolume);
    mStarsAnimation.Init();
    
    mUfo.Init(container);
    mUfoAnimation.Init();
    mUfo.SetPosition(initialUfoPosition);
    mUfo.Hide();
    
    mFadeEffect.Reset();
    mGameScene.GetScene().GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void LevelCompletedController::Start() {
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mElapsedTime = 0.0f;
    mState = State::Waiting;
    
    if (mSmallText.IsAwesomeTextActive() || mSmallText.IsFantasticTextActive()) {
        mWaitTime = smallTextAnimationWaintTime;
    } else {
        mWaitTime = waitTime;
    }
    
    mEngine.GetAudio().FadeOutActiveTrack(musicFadeOutTime);
    
    auto totalNumMovesUsed = mGameLogic.GetMovesUsedIncludingCurrent();
    
    auto controlType =
        mGameLogic.IsUsingClickControls() ? ControlType::Click : ControlType::Gesture;
    
    auto numStars =
        ProgressService::CalculateNumStars(totalNumMovesUsed, mLevel->GetStarLimits(controlType));

    mUserServices.CompleteLevel(mLevel->GetId(), totalNumMovesUsed, numStars);
}

void LevelCompletedController::GoToObjectiveAchievedAnimationState() {
    mState = State::ObjectiveAchievedAnimation;
    mElapsedTime = 0.0f;

    auto& container = mGameScene.GetLevelCompletedEffectsContainer();
    auto& cameraPosition = mGameScene.GetCamera().GetSceneObject().GetTransform().GetPosition();
    auto containerPosition = container.GetTransform().GetPosition();
    containerPosition.y = cameraPosition.y;
    container.GetTransform().SetPosition(containerPosition);
    container.SetIsStatic(false);
    container.SetIsVisible(true);
 
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::AllCleared));
    
    StartLevelCompletedTextAnimation();
    
    auto& scene = mGameScene.GetScene();
    scene.GetRenderPass(static_cast<int>(GameScene::Layer::LevelCompletedFadeEffect))->SetIsEnabled(true);
    scene.GetRenderPass(static_cast<int>(GameScene::Layer::Stars))->SetIsEnabled(true);
}

void LevelCompletedController::StartLevelCompletedTextAnimation() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingText.StartBlocksClearedMessage();
            break;
        case Level::Objective::Build:
            mSlidingText.StartSlotsFilledMessage();
            break;
        case Level::Objective::BringDownTheAsteroid:
            mSlidingText.StartTheAsteroidIsDownMessage();
            break;
    }
}

LevelCompletedDialogController::Result LevelCompletedController::Update() {
    auto result = LevelCompletedDialogController::Result::None;
    
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState();
            break;
        case State::ObjectiveAchievedAnimation:
            UpdateInObjectiveAchievedAnimationState();
            break;
        case State::Confetti:
            UpdateInConfettiState();
            break;
        case State::ClearingLastBlocks:
            UpdateInClearingLastBlocksState();
            break;
        case State::SlidingOutFieldAnimation:
            UpdateInSlidingOutFieldAnimationState();
            break;
        case State::Fireworks:
            UpdateInFireworksState();
            break;
        case State::StarsAppearingAnimation:
            UpdateInStarsAppearingAnimationState();
            break;
        case State::LevelCompletedDialog:
            result = UpdateLevelCompletedDialog();
            break;
    }
    
    mUfoAnimation.Update();
    return result;
}

void LevelCompletedController::UpdateInWaitingState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mElapsedTime > mWaitTime) {
        GoToObjectiveAchievedAnimationState();
    }
}

void LevelCompletedController::UpdateInObjectiveAchievedAnimationState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mElapsedTime > confettiWaitTime) {
        mConfettiParticleEffect.Start();
        mState = State::Confetti;
    }

    UpdateObjectiveAchievedAnimation();
}

void LevelCompletedController::UpdateObjectiveAchievedAnimation() {
    if (mSlidingText.Update() == SlidingText::State::Inactive) {
        if (mState == State::ObjectiveAchievedAnimation) {
            mConfettiParticleEffect.Start();
        }

        if (mLevel->GetObjective() == Level::Objective::Clear) {
            mState = State::ClearingLastBlocks;
            mClearLastBlocksAnimation.Start();
        } else {
            mState = State::SlidingOutFieldAnimation;
            mSlidingFieldAnimation.Start();
        }
    }
}

void LevelCompletedController::UpdateInConfettiState() {
    mConfettiParticleEffect.Update();
    UpdateObjectiveAchievedAnimation();
}

void LevelCompletedController::UpdateInClearingLastBlocksState() {
    mConfettiParticleEffect.Update();

    if (mClearLastBlocksAnimation.Update(mEngine.GetLastFrameSeconds()) ==
        ClearLastBlocksAnimation::State::Inactive) {

        mState = State::SlidingOutFieldAnimation;
        mSlidingFieldAnimation.Start();
    }
}

void LevelCompletedController::UpdateInSlidingOutFieldAnimationState() {
    mConfettiParticleEffect.Update();

    if (mSlidingFieldAnimation.Update() == SlidingFieldAnimation::State::Inactive) {
        mState = State::Fireworks;
        mElapsedTime = 0.0f;
        mFireworksParticleEffect.Start();
        mFadeEffect.Start();
        mEngine.GetRenderer().EnableShader(Pht::ShaderId::TexturedEnvMapLighting);
        mUfo.Show();
        mUfoAnimation.Start(ufoPosition);
    }
}

void LevelCompletedController::UpdateInFireworksState() {
    mFadeEffect.UpdateFadeOut(mEngine.GetLastFrameSeconds());
    UpdateFireworksAndConfetti();
}

void LevelCompletedController::UpdateFireworksAndConfetti() {
    auto fireworkState = mFireworksParticleEffect.Update();
    auto confettiState = mConfettiParticleEffect.Update();

    if (mState == State::Fireworks) {
        auto effectsAreDone =
            fireworkState == FireworksParticleEffect::State::Inactive &&
            confettiState == ConfettiParticleEffect::State::Inactive;
        
        mElapsedTime += mEngine.GetLastFrameSeconds();
        if (effectsAreDone || mElapsedTime > fireworksDuration ||
            mEngine.GetInput().ConsumeWholeTouch()) {
            
            auto controlType =
                mGameLogic.IsUsingClickControls() ? ControlType::Click : ControlType::Gesture;
            
            auto numStars =
                ProgressService::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                                   mLevel->GetStarLimits(controlType));

            mStarsAnimation.Start(numStars);
            mState = State::StarsAppearingAnimation;
        }
    }
}

void LevelCompletedController::UpdateInStarsAppearingAnimationState() {
    mFadeEffect.UpdateFadeOut(mEngine.GetLastFrameSeconds());
    UpdateFireworksAndConfetti();

    if (mStarsAnimation.Update() == StarsAnimation::State::Rotating) {
        mState = State::LevelCompletedDialog;
        mGameViewControllers.SetActiveController(GameViewControllers::LevelCompletedDialog);
        mGameViewControllers.GetLevelCompletedDialogController().SetUp();
        mStarsAnimation.MoveToFront();
        
        auto& audio = mEngine.GetAudio();
        audio.PlaySoundWithDelay(static_cast<Pht::AudioResourceId>(SoundId::LevelCompleted), 0.25f);
    }
}

LevelCompletedDialogController::Result LevelCompletedController::UpdateLevelCompletedDialog() {
    UpdateFireworksAndConfetti();
    mStarsAnimation.Update();
    mFadeEffect.Update(mEngine.GetLastFrameSeconds());

    return mGameViewControllers.GetLevelCompletedDialogController().Update();
}
