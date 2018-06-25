#include "LevelCompletedController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "Scene.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "GameViewControllers.hpp"
#include "SlidingTextAnimation.hpp"
#include "ClearLastBlocksAnimation.hpp"
#include "GameLogic.hpp"
#include "UserData.hpp"
#include "Level.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
    constexpr auto effectsVolumeDepth {20.0f};
    constexpr auto fireworksAndConfettiDuration {4.5f};
}

LevelCompletedController::LevelCompletedController(Pht::IEngine& engine,
                                                   GameScene& gameScene,
                                                   GameViewControllers& gameViewControllers,
                                                   SlidingTextAnimation& slidingTextAnimation,
                                                   ClearLastBlocksAnimation& clearLastBlocksAnimation,
                                                   GameLogic& gameLogic,
                                                   UserData& userData,
                                                   const CommonResources& commonResources) :
    mEngine {engine},
    mGameScene {gameScene},
    mGameViewControllers {gameViewControllers},
    mSlidingTextAnimation {slidingTextAnimation},
    mClearLastBlocksAnimation {clearLastBlocksAnimation},
    mGameLogic {gameLogic},
    mUserData {userData},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        -effectsVolumeDepth / 2.0f
    },
    mSlidingFieldAnimation {engine, gameScene},
    mFireworksParticleEffect {engine},
    mConfettiParticleEffect {engine},
    mStarsAnimation {engine, gameScene, commonResources} {
    
    mFadeEffect.GetSceneObject().SetLayer(static_cast<int>(GameScene::Layer::LevelCompletedFadeEffect));
}

void LevelCompletedController::Init(const Level& level) {
    mLevel = &level;
    
    auto& orthographicFrustumSize {mEngine.GetRenderer().GetOrthographicFrustumSize()};

    Pht::Vec3 effectsVolume {
        orthographicFrustumSize.x, orthographicFrustumSize.y, effectsVolumeDepth
    };
    
    auto& container {mGameScene.GetLevelCompletedEffectsContainer()};
    container.SetIsStatic(true);
    container.SetIsVisible(false);
    
    mFireworksParticleEffect.Init(container, effectsVolume);
    mConfettiParticleEffect.Init(container, effectsVolume);
    mStarsAnimation.Init();
    
    mFadeEffect.Reset();
    mGameScene.GetScene().GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void LevelCompletedController::Start() {
    mState = State::ObjectiveAchievedAnimation;
    mTimeSpentInFireworksAndConfettiState = 0.0f;

    auto& container {mGameScene.GetLevelCompletedEffectsContainer()};
    auto& cameraPosition {mGameScene.GetCamera().GetSceneObject().GetTransform().GetPosition()};
    auto containerPosition {container.GetTransform().GetPosition()};
    containerPosition.y = cameraPosition.y;
    container.GetTransform().SetPosition(containerPosition);
    container.SetIsStatic(false);
    container.SetIsVisible(true);

    mGameViewControllers.SetActiveController(GameViewControllers::None);
    StartLevelCompletedTextAnimation();
}

void LevelCompletedController::StartLevelCompletedTextAnimation() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::BlocksCleared);
            break;
        case Level::Objective::Build:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::SlotsFilled);
            break;
    }
}

LevelCompletedDialogController::Result LevelCompletedController::Update() {
    auto result {LevelCompletedDialogController::Result::None};
    
    switch (mState) {
        case State::ObjectiveAchievedAnimation:
            UpdateInObjectiveAchievedAnimationState();
            break;
        case State::ClearingLastBlocks:
            UpdateInClearingLastBlocksState();
            break;
        case State::SlidingOutFieldAnimation:
            UpdateInSlidingOutFieldAnimationState();
            break;
        case State::FireworksAndConfetti:
            if (mFadeEffect.GetState() != Pht::FadeEffect::State::Transition) {
                mFadeEffect.Update(mEngine.GetLastFrameSeconds());
            }
            UpdateFireworksAndConfetti();
            break;
        case State::StarsAppearingAnimation:
            UpdateInStarsAppearingAnimationState();
            break;
        case State::LevelCompletedDialog:
            result = UpdateLevelCompletedDialog();
            break;
    }
    
    return result;
}

void LevelCompletedController::UpdateInObjectiveAchievedAnimationState() {
    if (mSlidingTextAnimation.Update() == SlidingTextAnimation::State::Inactive) {
        if (mLevel->GetObjective() == Level::Objective::Clear) {
            mState = State::ClearingLastBlocks;
            mClearLastBlocksAnimation.Start();
        } else {
            mState = State::SlidingOutFieldAnimation;
            mSlidingFieldAnimation.Start();
        }
    }
}

void LevelCompletedController::UpdateInClearingLastBlocksState() {
    if (mClearLastBlocksAnimation.Update(mEngine.GetLastFrameSeconds()) ==
        ClearLastBlocksAnimation::State::Inactive) {

        mState = State::SlidingOutFieldAnimation;
        mSlidingFieldAnimation.Start();
    }
}

void LevelCompletedController::UpdateInSlidingOutFieldAnimationState() {
    if (mSlidingFieldAnimation.Update() == SlidingFieldAnimation::State::Inactive) {
        mState = State::FireworksAndConfetti;
        mFireworksParticleEffect.Start();
        mConfettiParticleEffect.Start();
        mFadeEffect.Start();
    }
}

void LevelCompletedController::UpdateFireworksAndConfetti() {
    auto fireworkState {mFireworksParticleEffect.Update()};
    auto confettiState {mConfettiParticleEffect.Update()};

    if (mState == State::FireworksAndConfetti) {
        auto effectsAreDone {
            fireworkState == FireworksParticleEffect::State::Inactive &&
            confettiState == ConfettiParticleEffect::State::Inactive
        };
        
        mTimeSpentInFireworksAndConfettiState += mEngine.GetLastFrameSeconds();
        
        if (effectsAreDone || mTimeSpentInFireworksAndConfettiState > fireworksAndConfettiDuration ||
            mEngine.GetInput().ConsumeWholeTouch()) {

            auto numStars {
                ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                                   mLevel->GetStarLimits())
            };

            mStarsAnimation.Start(numStars);
            mState = State::StarsAppearingAnimation;
            mFadeEffect.GetSceneObject().GetRenderable()->GetMaterial().SetOpacity(fade);
        }
    }
}

void LevelCompletedController::UpdateInStarsAppearingAnimationState() {
    UpdateFireworksAndConfetti();

    if (mStarsAnimation.Update() == StarsAnimation::State::Rotating) {
        mState = State::LevelCompletedDialog;
        mGameViewControllers.SetActiveController(GameViewControllers::LevelCompletedDialog);
        mGameViewControllers.GetLevelCompletedDialogController().Init();
        mStarsAnimation.MoveToFront();
    }
}

LevelCompletedDialogController::Result LevelCompletedController::UpdateLevelCompletedDialog() {
    UpdateFireworksAndConfetti();
    mStarsAnimation.Update();
    
    if (mFadeEffect.GetState() != Pht::FadeEffect::State::Idle) {
        mFadeEffect.Update(mEngine.GetLastFrameSeconds());
    }

    auto result {mGameViewControllers.GetLevelCompletedDialogController().Update()};
    
    if (result != LevelCompletedDialogController::Result::None) {
        auto numStars {
            ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                               mLevel->GetStarLimits())
        };
        
        mUserData.CompleteLevel(mLevel->GetId(), numStars);
    }
    
    return result;
}
