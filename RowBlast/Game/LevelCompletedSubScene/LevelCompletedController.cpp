#include "LevelCompletedController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
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
    constexpr auto fade {0.6f};
    constexpr auto fadeTime {0.3f};
    constexpr auto effectsVolumeDepth {20.0f};
}

LevelCompletedController::LevelCompletedController(Pht::IEngine& engine,
                                                   GameScene& gameScene,
                                                   GameViewControllers& gameViewControllers,
                                                   SlidingTextAnimation& slidingTextAnimation,
                                                   ClearLastBlocksAnimation& clearLastBlocksAnimation,
                                                   GameLogic& gameLogic,
                                                   UserData& userData) :
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
    mConfettiParticleEffect {engine} {
    
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
    
    mFadeEffect.Reset();
    mGameScene.GetScene().GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void LevelCompletedController::Start() {
    mState = State::ObjectiveAchievedAnimation;

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
            UpdateInFireworksAndConfettiState();
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

void LevelCompletedController::UpdateInFireworksAndConfettiState() {
    if (mFadeEffect.GetState() != Pht::FadeEffect::State::Transition) {
        mFadeEffect.Update(mEngine.GetLastFrameSeconds());
    }

    auto fireworkState {mFireworksParticleEffect.Update()};
    auto confettiState {mConfettiParticleEffect.Update()};

    if (fireworkState == FireworksParticleEffect::State::Inactive &&
        confettiState == ConfettiParticleEffect::State::Inactive) {

        GoToLevelCompletedDialogState();
    }
}

void LevelCompletedController::GoToLevelCompletedDialogState() {
    mState = State::LevelCompletedDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::LevelCompletedDialog);
    
    auto& levelCompletedDialogController {
        mGameViewControllers.GetLevelCompletedDialogController()
    };
    
    levelCompletedDialogController.Init();
    
    auto numStars {
        ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                           mLevel->GetStarLimits())
    };
    
    levelCompletedDialogController.GetView().SetNumStars(numStars);
}

LevelCompletedDialogController::Result LevelCompletedController::UpdateLevelCompletedDialog() {
    auto result {mGameViewControllers.GetLevelCompletedDialogController().Update()};
    
    if (result != LevelCompletedDialogController::Result::None) {
        auto numStars {
            ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                               mLevel->GetStarLimits())
        };
        
        mUserData.CompleteLevel(mLevel->GetIndex(), numStars);
    }
    
    return result;
}
