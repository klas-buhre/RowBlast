#include "GameController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "LevelLoader.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"
#include "CommonResources.hpp"

using namespace BlocksGame;

namespace {
    bool ShouldUpdateGameLogic(CollapsingFieldAnimation::State fieldAnimationState,
                               FallingPieceAnimation::State fallingPieceAnimationState,
                               ExplosionParticleEffect::State explosionAnimationState,
                               RowExplosionParticleEffect::State rowExplosionAnimationState,
                               ScrollController::State scrollState) {
        return fieldAnimationState == CollapsingFieldAnimation::State::Inactive &&
               fallingPieceAnimationState == FallingPieceAnimation::State::Inactive &&
               explosionAnimationState == ExplosionParticleEffect::State::Inactive &&
               rowExplosionAnimationState == RowExplosionParticleEffect::State::Inactive &&
               scrollState != ScrollController::State::Scrolling &&
               scrollState != ScrollController::State::LevelOverviewScroll &&
               scrollState != ScrollController::State::BeforeLevelOverviewScroll;
    }
}

GameController::GameController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               CommonViewControllers& commonViewControllers,
                               UserData& userData,
                               Settings& settings) :
    mEngine {engine},
    mUserData {userData},
    mGameViewControllers {engine, commonResources, mUserData, commonViewControllers},
    mField {},
    mCollapsingFieldAnimation {mField},
    mFlashingBlocksAnimation {mField},
    mScrollController {engine, mField},
    mScene {
        engine,
        mScrollController,
        commonResources,
        mGameViewControllers.GetGameHudController()
    },
    mExplosionParticleEffect {engine, mScene},
    mRowExplosionParticleEffect {engine, mScene},
    mPieceDropParticleEffect {engine, mScene},
    mBlastRadiusAnimation {engine, mScene},
    mSlidingTextAnimation {engine, mScene},
    mClearLastBlocksAnimation {mField, mFlyingBlocksAnimation},
    mGameLogic {
        engine,
        mField,
        mScrollController,
        mScene,
        mExplosionParticleEffect,
        mRowExplosionParticleEffect,
        mFlyingBlocksAnimation,
        mPieceDropParticleEffect,
        mBlastRadiusAnimation,
        mGameViewControllers.GetGameHudController(),
        settings
    },
    mFallingPieceAnimation {mGameLogic.GetFallingPieceAnimation()},
    mPieceResources {engine, mScene},
    mLevelResources {engine, mScene},
    mFlyingBlocksAnimation {mScene, mLevelResources, mPieceResources},
    mBlueprintSlotsFilledAnimation {mField, mScene, mLevelResources},
    mHud {
        engine,
        mGameLogic,
        mLevelResources,
        mGameViewControllers.GetGameHudController(),
        commonResources.GetHussarFontSize22()
    },
    mRenderer {
        engine,
        mScene,
        mField,
        mGameLogic,
        mScrollController,
        mHud,
        mGameViewControllers,
        mPieceResources,
        mLevelResources
    } {}

void GameController::StartLevel(int levelIndex) {
    mLevel = LevelLoader::Load(levelIndex, mLevelResources);

    mField.Init(*mLevel);
    mScrollController.Init(mLevel->GetObjective());
    mScene.Init(*mLevel, mLevelResources, mPieceResources, mGameLogic);
    mGameLogic.Init(*mLevel);
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mHud.Init(*mLevel);
    mBlueprintSlotsFilledAnimation.Init();
    mPieceDropParticleEffect.Init();
    mBlastRadiusAnimation.Init();
    mExplosionParticleEffect.Init();
    mRowExplosionParticleEffect.Init();
    mFlyingBlocksAnimation.Init();
    mSlidingTextAnimation.Init();
    
    mState = GameState::LevelIntro;
    mLevelIntroState = LevelIntroState::Overview;
    mLevelCompletedState = LevelCompletedState::ObjectiveAchievedAnimation;
    
    mUserData.StartLevel(levelIndex);
}

GameController::Command GameController::Update() {
    auto command {Command::None};
    
    switch (mState) {
        case GameState::LevelIntro:
        case GameState::Playing:
        case GameState::GameOver:
        case GameState::LevelCompleted:
        case GameState::OutOfMoves:
            command = UpdateGame();
            break;
        case GameState::Paused:
            command = UpdateInPausedState();
            break;
    }
    
    mRenderer.RenderFrame();
    mField.OnEndOfFrame();
    
    return command;
}

void GameController::RenderScene() {
    mRenderer.Render();
}

GameController::Command GameController::UpdateGame() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    auto scrollState {mScrollController.Update()};
    auto fieldAnimationState {mCollapsingFieldAnimation.Update(dt)};
    mBlueprintSlotsFilledAnimation.Update(dt);
    auto explosionState {mExplosionParticleEffect.Update(dt)};
    auto rowExplosionState {mRowExplosionParticleEffect.Update(dt)};
    auto fallingPieceAnimationState {mFallingPieceAnimation.Update(dt)};
    
    if (mState == GameState::Playing) {
        mShouldUpdateGameLogic = ShouldUpdateGameLogic(fieldAnimationState,
                                                       fallingPieceAnimationState,
                                                       explosionState,
                                                       rowExplosionState,
                                                       scrollState);
        auto result {mGameLogic.Update(mShouldUpdateGameLogic)};

        if (result != GameLogic::Result::None) {
            ChangeGameState(result);
        }
    }
    
    auto command {UpdateSubState()};
    
    mFlashingBlocksAnimation.Update(dt);
    mPieceDropParticleEffect.Update(dt);
    mBlastRadiusAnimation.Update(dt);
    mFlyingBlocksAnimation.Update(dt);
    mHud.Update();
    mScene.Update();
    
    return command;
}

void GameController::ChangeGameState(GameLogic::Result gameLogicResult) {
    switch (gameLogicResult) {
        case GameLogic::Result::Pause:
            mState = GameState::Paused;
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::Yes);
            break;
        case GameLogic::Result::OutOfMoves:
            mState = GameState::OutOfMoves;
            mGameViewControllers.SetActiveController(GameViewControllers::NoMovesDialog);
            mGameViewControllers.GetNoMovesDialogController().Init();
            break;
        case GameLogic::Result::GameOver:
            mState = GameState::GameOver;
            GoToGameOverStateGameOverDialog();
            mUserData.GetLifeManager().FailLevel();
            break;
        case GameLogic::Result::LevelCompleted:
            mState = GameState::LevelCompleted;
            mGameViewControllers.SetActiveController(GameViewControllers::None);
            StartLevelCompletedAnimation();
            break;
        default:
            break;
    }
}

GameController::Command GameController::UpdateInPausedState() {
    auto command {Command::None};
    
    switch (mPausedState) {
        case PausedState::GameMenu:
            UpdateGameMenu();
            break;
        case PausedState::SettingsMenu:
            UpdateSettingsMenu();
            break;
        case PausedState::NoLivesDialog:
            command = UpdateNoLivesDialog();
            break;
        case PausedState::RestartConfirmationDialog:
            command = UpdateRestartConfirmationDialog();
            break;
        case PausedState::MapConfirmationDialog:
            command = UpdateMapConfirmationDialog();
            break;
    }
    
    mUserData.Update();
    
    return command;
}

void GameController::UpdateGameMenu() {
    switch (mGameViewControllers.GetGameMenuController().Update()) {
        case GameMenuController::Result::None:
            break;
        case GameMenuController::Result::ResumeGame:
            GoToPlayingState();
            break;
        case GameMenuController::Result::UndoMove:
            mGameLogic.UndoMove();
            break;
        case GameMenuController::Result::RestartGame:
            GoToPausedStateRestartConfirmationDialog();
            break;
        case GameMenuController::Result::GoToSettingsMenu:
            GoToPausedStateSettingsMenu();
            break;
        case GameMenuController::Result::BackToMap:
            GoToPausedStateMapConfirmationDialog();
            break;
    }
}

void GameController::UpdateSettingsMenu() {
    switch (mGameViewControllers.GetSettingsMenuController().Update()) {
        case SettingsMenuController::Result::None:
            break;
        case SettingsMenuController::Result::GoBack:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No);
            break;
    }
}

GameController::Command GameController::UpdateNoLivesDialog() {
    auto command {Command::None};

    switch (mGameViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            mUserData.GetLifeManager().RefillLives();
            command = Command::RestartGame;
            break;
        case NoLivesDialogController::Result::Close:
            if (mState == GameState::Paused) {
                GoToPlayingState();
            } else {
                command = Command::GoToMap;
            }
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateRestartConfirmationDialog() {
    auto command {Command::None};
    
    switch (mGameViewControllers.GetRestartConfirmationDialogController().Update()) {
        case RestartConfirmationDialogController::Result::None:
            break;
        case RestartConfirmationDialogController::Result::RestartGame: {
            auto& lifeManager {mUserData.GetLifeManager()};
            lifeManager.FailLevel();
            if (lifeManager.GetNumLives() == 0) {
                GoToPausedStateNoLivesDialog();
            } else {
                command = Command::RestartGame;
            }
            break;
        }
        case RestartConfirmationDialogController::Result::DoNotRestartGame:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No);
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateMapConfirmationDialog() {
    auto command {Command::None};
    
    switch (mGameViewControllers.GetMapConfirmationDialogController().Update()) {
        case MapConfirmationDialogController::Result::None:
            break;
        case MapConfirmationDialogController::Result::GoToMap: {
            mUserData.GetLifeManager().FailLevel();
            command = Command::GoToMap;
            break;
        }
        case MapConfirmationDialogController::Result::DoNotGoToMap:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No);
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateSubState() {
    auto command {Command::None};
    
    switch (mState) {
        case GameState::LevelIntro:
            UpdateInLevelIntroState();
            break;
        case GameState::LevelCompleted:
            command = UpdateInLevelCompletedState();
            break;
        case GameState::OutOfMoves:
            command = UpdateNoMovesDialog();
            break;
        case GameState::GameOver:
            command = UpdateInGameOverState();
            break;
        default:
            break;
    }
    
    return command;
}

void GameController::UpdateInLevelIntroState() {
    switch (mLevelIntroState) {
        case LevelIntroState::Overview:
            if (mScrollController.GetState() == ScrollController::State::Idle) {
                StartLevelObjectiveAnimation();
            }
            break;
        case LevelIntroState::ObjectiveAnimation:
            if (mSlidingTextAnimation.Update() == SlidingTextAnimation::State::Inactive) {
                GoToPlayingState();
            }
            break;
    }
}

void GameController::StartLevelObjectiveAnimation() {
    mLevelIntroState = LevelIntroState::ObjectiveAnimation;

    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::ClearBlocks);
            break;
        case Level::Objective::Build:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::FillSlots);
            break;
    }
}

GameController::Command GameController::UpdateNoMovesDialog() {
    auto command {Command::None};
    
    mUserData.Update();
    
    switch (mGameViewControllers.GetNoMovesDialogController().Update()) {
        case NoMovesDialogController::Result::None:
            break;
        case NoMovesDialogController::Result::PlayOn:
            mGameLogic.SetMovesLeft(5);
            GoToPlayingState();
            break;
        case NoMovesDialogController::Result::BackToMap:
            mUserData.GetLifeManager().FailLevel();
            command = Command::GoToMap;
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInGameOverState() {
    auto command {Command::None};
    
    mUserData.Update();
    
    switch (mGameOverState) {
        case GameOverState::GameOverDialog:
            command = UpdateGameOverDialog();
            break;
        case GameOverState::NoLivesDialog:
            command = UpdateNoLivesDialog();
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateGameOverDialog() {
    auto command {Command::None};
    
    switch (mGameViewControllers.GetGameOverDialogController().Update()) {
        case GameOverDialogController::Result::None:
            break;
        case GameOverDialogController::Result::Retry:
            if (mUserData.GetLifeManager().GetNumLives() == 0) {
                GoToGameOverStateNoLivesDialog();
            } else {
                command = Command::RestartGame;
            }
            break;
        case GameOverDialogController::Result::BackToMap:
            command = Command::GoToMap;
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInLevelCompletedState() {
    auto command {Command::None};
    
    switch (mLevelCompletedState) {
        case LevelCompletedState::ObjectiveAchievedAnimation:
            if (mSlidingTextAnimation.Update() == SlidingTextAnimation::State::Inactive) {
                if (mLevel->GetObjective() == Level::Objective::Clear) {
                    mLevelCompletedState = LevelCompletedState::ClearingLastBlocks;
                    mClearLastBlocksAnimation.Start();
                } else {
                    GoToLevelCompletedStateLevelCompletedDialog();
                }
            }
            break;
        case LevelCompletedState::ClearingLastBlocks:
            if (mClearLastBlocksAnimation.Update(mEngine.GetLastFrameSeconds()) ==
                ClearLastBlocksAnimation::State::Inactive) {
                GoToLevelCompletedStateLevelCompletedDialog();
            }
            break;
        case LevelCompletedState::LevelCompletedDialog:
            command = UpdateLevelCompletedDialog();
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateLevelCompletedDialog() {
    auto command {Command::None};
    auto result {mGameViewControllers.GetLevelCompletedDialogController().Update()};
    
    switch (result) {
        case LevelCompletedDialogController::Result::None:
            break;
        case LevelCompletedDialogController::Result::NextLevel:
            command = Command::GoToNextLevel;
            break;
        case LevelCompletedDialogController::Result::BackToMap:
            command = Command::GoToMap;
            break;
    }
    
    if (result != LevelCompletedDialogController::Result::None) {
        auto numStars {
            ProgressManager::CalculateNumStars(mGameLogic.GetMovesUsedIncludingCurrent(),
                                               mLevel->GetStarLimits())
        };
        
        mUserData.CompleteLevel(mLevel->GetIndex(), numStars);
    }
    
    return command;
}

void GameController::StartLevelCompletedAnimation() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::BlocksCleared);
            break;
        case Level::Objective::Build:
            mSlidingTextAnimation.Start(SlidingTextAnimation::Message::SlotsFilled);
            break;
    }
}

void GameController::GoToPlayingState() {
    mState = GameState::Playing;
    mGameViewControllers.SetActiveController(GameViewControllers::GameHud);
}

void GameController::GoToPausedStateNoLivesDialog() {
    mPausedState = PausedState::NoLivesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::NoLivesDialog);
    mGameViewControllers.GetNoLivesDialogController().Init(SlidingMenuAnimation::UpdateFade::Yes,
                                                           true);
}

void GameController::GoToPausedStateRestartConfirmationDialog() {
    mPausedState = PausedState::RestartConfirmationDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::RestartConfirmationDialog);
    mGameViewControllers.GetRestartConfirmationDialogController().Init();
}

void GameController::GoToPausedStateMapConfirmationDialog() {
    mPausedState = PausedState::MapConfirmationDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::MapConfirmationDialog);
    mGameViewControllers.GetMapConfirmationDialogController().Init();
}

void GameController::GoToPausedStateSettingsMenu() {
    mPausedState = PausedState::SettingsMenu;
    mGameViewControllers.SetActiveController(GameViewControllers::SettingsMenu);
    mGameViewControllers.GetSettingsMenuController().Init(SlidingMenuAnimation::UpdateFade::No);
}

void GameController::GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade) {
    mPausedState = PausedState::GameMenu;
    mGameViewControllers.SetActiveController(GameViewControllers::GameMenu);
    auto isUndoMovePossible {mShouldUpdateGameLogic && mGameLogic.IsUndoMovePossible()};
    mGameViewControllers.GetGameMenuController().Init(updateFade, isUndoMovePossible);
}

void GameController::GoToLevelCompletedStateLevelCompletedDialog() {
    mLevelCompletedState = LevelCompletedState::LevelCompletedDialog;
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

void GameController::GoToGameOverStateGameOverDialog() {
    mGameOverState = GameOverState::GameOverDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::GameOverDialog);
    mGameViewControllers.GetGameOverDialogController().Init();
}

void GameController::GoToGameOverStateNoLivesDialog() {
    mGameOverState = GameOverState::NoLivesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::NoLivesDialog);
    mGameViewControllers.GetNoLivesDialogController().Init(SlidingMenuAnimation::UpdateFade::Yes,
                                                           false);
}
