#include "LevelCompletedController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "GameViewControllers.hpp"
#include "SlidingTextAnimation.hpp"
#include "ClearLastBlocksAnimation.hpp"
#include "GameLogic.hpp"
#include "UserData.hpp"
#include "Level.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

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
    mSlidingFieldAnimation {engine, gameScene},
    mFireworksParticleEffect {engine} {}

void LevelCompletedController::Init(const Level& level) {
    mLevel = &level;
    
    auto& orthographicFrustumSize {mEngine.GetRenderer().GetOrthographicFrustumSize()};
    Pht::Vec3 effectsVolume {orthographicFrustumSize.x, orthographicFrustumSize.y, 15.0f};
    
    auto& container {mGameScene.GetLevelCompletedEffectsContainer()};
    container.SetIsStatic(true);
    container.SetIsVisible(false);
    
    mFireworksParticleEffect.Init(container, effectsVolume);
}

void LevelCompletedController::Start() {
    mState = State::ObjectiveAchievedAnimation;

    auto& container {mGameScene.GetLevelCompletedEffectsContainer()};
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
            if (mSlidingTextAnimation.Update() == SlidingTextAnimation::State::Inactive) {
                if (mLevel->GetObjective() == Level::Objective::Clear) {
                    mState = State::ClearingLastBlocks;
                    mClearLastBlocksAnimation.Start();
                } else {
                    GoToLevelCompletedDialogState();
                }
            }
            break;
        case State::ClearingLastBlocks:
            if (mClearLastBlocksAnimation.Update(mEngine.GetLastFrameSeconds()) ==
                ClearLastBlocksAnimation::State::Inactive) {
                mState = State::SlidingOutFieldAnimation;
                mSlidingFieldAnimation.Start();
            }
            break;
        case State::SlidingOutFieldAnimation:
            if (mSlidingFieldAnimation.Update() == SlidingFieldAnimation::State::Inactive) {
                mState = State::Fireworks;
                mFireworksParticleEffect.Start();
            }
            break;
        case State::Fireworks:
            if (mFireworksParticleEffect.Update() == FireworksParticleEffect::State::Inactive) {
                GoToLevelCompletedDialogState();
            }
            break;
        case State::LevelCompletedDialog:
            result = UpdateLevelCompletedDialog();
            break;
    }
    
    return result;
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
