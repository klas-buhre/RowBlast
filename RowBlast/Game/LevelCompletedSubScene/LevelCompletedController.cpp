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
#include "GameLogic.hpp"
#include "UserData.hpp"
#include "Level.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
    constexpr auto effectsVolumeDepth {20.0f};
    constexpr auto fireworksDuration {2.0f};
    constexpr auto confettiWaitTime {0.85f};
}

LevelCompletedController::LevelCompletedController(Pht::IEngine& engine,
                                                   GameScene& gameScene,
                                                   GameViewControllers& gameViewControllers,
                                                   SlidingTextAnimation& slidingTextAnimation,
                                                   GameLogic& gameLogic,
                                                   UserData& userData,
                                                   const CommonResources& commonResources,
                                                   Pht::CameraShake& cameraShake,
                                                   Field& field,
                                                   FlyingBlocksAnimation& flyingBlocksAnimation) :
    mEngine {engine},
    mGameScene {gameScene},
    mGameViewControllers {gameViewControllers},
    mSlidingTextAnimation {slidingTextAnimation},
    mGameLogic {gameLogic},
    mUserData {userData},
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
    mStarsAnimation {engine, gameScene, commonResources, cameraShake} {
    
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
    mElapsedTime = 0.0f;

    auto& container {mGameScene.GetLevelCompletedEffectsContainer()};
    auto& cameraPosition {mGameScene.GetCamera().GetSceneObject().GetTransform().GetPosition()};
    auto containerPosition {container.GetTransform().GetPosition()};
    containerPosition.y = cameraPosition.y;
    container.GetTransform().SetPosition(containerPosition);
    container.SetIsStatic(false);
    container.SetIsVisible(true);

    mGameViewControllers.SetActiveController(GameViewControllers::None);
    StartLevelCompletedTextAnimation();
    
    auto& scene {mGameScene.GetScene()};
    scene.GetRenderPass(static_cast<int>(GameScene::Layer::LevelCompletedFadeEffect))->SetIsEnabled(true);
    scene.GetRenderPass(static_cast<int>(GameScene::Layer::Stars))->SetIsEnabled(true);
}

void LevelCompletedController::StartLevelCompletedTextAnimation() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::BlocksCleared);
            break;
        case Level::Objective::Build:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::SlotsFilled);
            break;
        case Level::Objective::BringDownTheAsteroid:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::TheAsteroidIsDown);
            break;
    }
}

LevelCompletedDialogController::Result LevelCompletedController::Update() {
    auto result {LevelCompletedDialogController::Result::None};
    
    switch (mState) {
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
    
    return result;
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
    if (mSlidingTextAnimation.Update() == SlidingTextAnimation::State::Inactive) {
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
    }
}

void LevelCompletedController::UpdateInFireworksState() {
    mFadeEffect.UpdateFadeOut(mEngine.GetLastFrameSeconds());
    UpdateFireworksAndConfetti();
}

void LevelCompletedController::UpdateFireworksAndConfetti() {
    auto fireworkState {mFireworksParticleEffect.Update()};
    auto confettiState {mConfettiParticleEffect.Update()};

    if (mState == State::Fireworks) {
        auto effectsAreDone {
            fireworkState == FireworksParticleEffect::State::Inactive &&
            confettiState == ConfettiParticleEffect::State::Inactive
        };
        
        mElapsedTime += mEngine.GetLastFrameSeconds();
        
        if (effectsAreDone || mElapsedTime > fireworksDuration ||
            mEngine.GetInput().ConsumeWholeTouch()) {

            auto numStars {
                ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                                   mLevel->GetStarLimits())
            };

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
        mGameViewControllers.GetLevelCompletedDialogController().Init();
        mStarsAnimation.MoveToFront();
    }
}

LevelCompletedDialogController::Result LevelCompletedController::UpdateLevelCompletedDialog() {
    UpdateFireworksAndConfetti();
    mStarsAnimation.Update();
    
    mFadeEffect.Update(mEngine.GetLastFrameSeconds());

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
