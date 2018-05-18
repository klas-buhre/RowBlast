#include "LevelCompletedController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameViewControllers.hpp"
#include "SlidingTextAnimation.hpp"
#include "ClearLastBlocksAnimation.hpp"
#include "GameLogic.hpp"
#include "UserData.hpp"
#include "Level.hpp"

using namespace RowBlast;

LevelCompletedController::LevelCompletedController(Pht::IEngine& engine,
                                                   GameScene& gameScene,
                                                   GameViewControllers& gameViewControllers,
                                                   SlidingTextAnimation& slidingTextAnimation,
                                                   ClearLastBlocksAnimation& clearLastBlocksAnimation,
                                                   GameLogic& gameLogic,
                                                   UserData& userData) :
    mEngine {engine},
    mGameViewControllers {gameViewControllers},
    mSlidingTextAnimation {slidingTextAnimation},
    mClearLastBlocksAnimation {clearLastBlocksAnimation},
    mGameLogic {gameLogic},
    mUserData {userData},
    mSlidingFieldAnimation {engine, gameScene} {}

void LevelCompletedController::Init(const Level& level) {
    mLevel = &level;
}

void LevelCompletedController::Start() {
    mState = State::ObjectiveAchievedAnimation;
    
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
