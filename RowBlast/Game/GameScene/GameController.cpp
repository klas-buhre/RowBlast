#include "GameController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "LevelLoader.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    bool ShouldUpdateGameLogic(CollapsingFieldAnimation::State fieldAnimationState,
                               FallingPieceAnimation::State fallingPieceAnimationState,
                               EffectManager::State effectsState,
                               ScrollController::State scrollState) {
        return (fieldAnimationState == CollapsingFieldAnimation::State::Inactive ||
                fieldAnimationState == CollapsingFieldAnimation::State::BlocksBouncing) &&
               fallingPieceAnimationState == FallingPieceAnimation::State::Inactive &&
               effectsState != EffectManager::State::OngoingEffects &&
               scrollState != ScrollController::State::Scrolling &&
               scrollState != ScrollController::State::LevelOverviewScroll &&
               scrollState != ScrollController::State::BeforeLevelOverviewScroll;
    }
}

GameController::GameController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               UserData& userData,
                               Settings& settings) :
    mEngine {engine},
    mUserData {userData},
    mPieceResources {engine, commonResources},
    mHudRectangles {engine, commonResources},
    mGameViewControllers {
        engine,
        commonResources,
        mUserData,
        settings,
        mPieceResources,
        mHudRectangles
    },
    mField {},
    mCollapsingFieldAnimation {mField},
    mFlashingBlocksAnimation {mField, mPieceResources},
    mWeldsAnimation {mField},
    mScrollController {engine, mField},
    mCameraShake {Pht::CameraShake::ShakeKind::PerlinNoise},
    mScene {
        engine,
        mScrollController,
        commonResources,
        mGameViewControllers.GetGameHudController(),
        mCameraShake,
        mHudRectangles
    },
    mEffectManager {engine, mScene, mCameraShake},
    mPieceDropParticleEffect {engine, mScene},
    mBlastRadiusAnimation {engine, mScene, commonResources},
    mSlidingTextAnimation {engine, mScene, commonResources},
    mComboTextAnimation {engine, mScene, commonResources},
    mClearLastBlocksAnimation {mField, mFlyingBlocksAnimation},
    mFallingPieceScaleAnimation {mScene},
    mTutorial {engine, mScene, commonResources},
    mGameLogic {
        engine,
        mField,
        mScrollController,
        mScene,
        mEffectManager,
        mFlyingBlocksAnimation,
        mFlashingBlocksAnimation,
        mCollapsingFieldAnimation,
        mPieceDropParticleEffect,
        mBlastRadiusAnimation,
        mFallingPieceScaleAnimation,
        mComboTextAnimation,
        mGameViewControllers.GetGameHudController(),
        mTutorial,
        settings
    },
    mFallingPieceAnimation {mGameLogic.GetFallingPieceAnimation()},
    mLevelResources {engine, mScene, commonResources},
    mPreviewPiecesAnimation {mScene, mGameLogic},
    mBlueprintSlotsFilledAnimation {mField, mScene, mLevelResources},
    mBombsAnimation {mScene, mPieceResources, mLevelResources},
    mFlyingBlocksAnimation {mScene, mLevelResources, mPieceResources, mBombsAnimation},
    mRenderer {
        mScene,
        mField,
        mGameLogic,
        mScrollController,
        mBombsAnimation,
        mPieceResources,
        mLevelResources
    },
    mLevelCompletedController {
        engine,
        mScene,
        mGameViewControllers,
        mSlidingTextAnimation,
        mClearLastBlocksAnimation,
        mGameLogic,
        mUserData,
        commonResources,
        mCameraShake
    } {}

void GameController::StartLevel(int levelId) {
    mLevel = LevelLoader::Load(levelId, mLevelResources);

    mField.Init(*mLevel);
    mScrollController.Init(mLevel->GetObjective());
    mScene.Init(*mLevel, mLevelResources, mPieceResources, mGameLogic);
    mTutorial.Init(*mLevel);
    mGameLogic.Init(*mLevel);
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mBlueprintSlotsFilledAnimation.Init();
    mPieceDropParticleEffect.Init();
    mBlastRadiusAnimation.Init();
    mEffectManager.Init();
    mCameraShake.Init();
    mPreviewPiecesAnimation.Init();
    mFlyingBlocksAnimation.Init();
    mFlashingBlocksAnimation.Init();
    mFallingPieceAnimation.Init();
    mSlidingTextAnimation.Init();
    mComboTextAnimation.Init();
    mBombsAnimation.Init();
    mFallingPieceScaleAnimation.Init();
    mGameViewControllers.Init(mScene);
    mLevelCompletedController.Init(*mLevel);
    
    mState = GameState::LevelIntro;
    mLevelIntroState = LevelIntroState::Overview;
    
    mUserData.StartLevel(levelId);
    
    mEngine.GetSceneManager().InitRenderer();
}

GameController::Command GameController::Update() {
    auto command {Command::None};
    
    switch (mState) {
        case GameState::LevelIntro:
        case GameState::TutorialDialog:
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
    
    mRenderer.Render();
    mField.OnEndOfFrame();
    
    return command;
}

GameController::Command GameController::UpdateGame() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    auto scrollState {mScrollController.Update()};
    auto fieldAnimationState {mCollapsingFieldAnimation.Update(dt)};
    mBlueprintSlotsFilledAnimation.Update(dt);
    auto effectsState {mEffectManager.GetState()};
    auto fallingPieceAnimationState {mFallingPieceAnimation.Update(dt)};
    
    if (mState == GameState::Playing) {
        mShouldUpdateGameLogic = ShouldUpdateGameLogic(fieldAnimationState,
                                                       fallingPieceAnimationState,
                                                       effectsState,
                                                       scrollState);
        auto result {mGameLogic.Update(mShouldUpdateGameLogic)};

        if (result != GameLogic::Result::None) {
            ChangeGameState(result);
        }
    }
    
    auto command {UpdateSubState()};
    
    mFlashingBlocksAnimation.Update(dt);
    mWeldsAnimation.Update(dt);
    mPieceDropParticleEffect.Update(dt);
    mBlastRadiusAnimation.Update(dt);
    mEffectManager.Update(dt);
    mCameraShake.Update(dt);
    mFlyingBlocksAnimation.Update(dt);
    mFallingPieceScaleAnimation.Update(dt);
    mScene.Update();
    mPreviewPiecesAnimation.Update(dt);
    mBombsAnimation.Update(dt);
    mComboTextAnimation.Update(dt);
    
    mTutorial.Update();
    
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
            mLevelCompletedController.Start();
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
        case PausedState::LevelGoalDialog:
            UpdateLevelGoalDialog();
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
    
    mScene.UpdateLightAnimation();
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
        case GameMenuController::Result::GoToLevelGoalDialog:
            GoToPausedStateLevelGoalDialog();
            break;
        case GameMenuController::Result::GoToSettingsMenu:
            GoToPausedStateSettingsMenu();
            break;
        case GameMenuController::Result::RestartGame:
            GoToPausedStateRestartConfirmationDialog();
            break;
        case GameMenuController::Result::BackToMap:
            GoToPausedStateMapConfirmationDialog();
            break;
    }
}

void GameController::UpdateLevelGoalDialog() {
    switch (mGameViewControllers.GetLevelGoalDialogController().Update()) {
        case LevelGoalDialogController::Result::None:
            break;
        case LevelGoalDialogController::Result::Play:
            assert(!"Unexpected result");
            break;
        case LevelGoalDialogController::Result::Close:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No);
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
            command = Command::RestartLevel;
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
        case RestartConfirmationDialogController::Result::RestartLevel: {
            auto& lifeManager {mUserData.GetLifeManager()};
            lifeManager.FailLevel();
            if (lifeManager.GetNumLives() == 0) {
                GoToPausedStateNoLivesDialog();
            } else {
                command = Command::RestartLevel;
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
        case GameState::TutorialDialog:
            UpdateTutorialDialogs();
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
                if (mTutorial.OnLevelStart() == Tutorial::Result::TutorialHasFocus) {
                    mState = GameState::TutorialDialog;
                } else {
                    GoToPlayingState();
                }
            }
            break;
    }
}

void GameController::UpdateTutorialDialogs() {
    if (mTutorial.UpdateDialogs() == Tutorial::Result::Play) {
        GoToPlayingState();
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

GameController::Command GameController::UpdateInLevelCompletedState() {
    auto command {Command::None};
    
    switch (mLevelCompletedController.Update()) {
        case LevelCompletedDialogController::Result::None:
            break;
        case LevelCompletedDialogController::Result::NextLevel:
            command = Command::GoToNextLevel;
            break;
        case LevelCompletedDialogController::Result::BackToMap:
            command = Command::GoToMap;
            break;
    }
    
    return command;
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
                command = Command::RestartLevel;
            }
            break;
        case GameOverDialogController::Result::BackToMap:
            command = Command::GoToMap;
            break;
    }
    
    return command;
}

void GameController::GoToPlayingState() {
    mState = GameState::Playing;
    mEngine.GetInput().EnableInput();
    mGameViewControllers.SetActiveController(GameViewControllers::GameHud);
    mTutorial.OnResumePlaying();
}

void GameController::GoToPausedStateNoLivesDialog() {
    mPausedState = PausedState::NoLivesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::NoLivesDialog);
    mGameViewControllers.GetNoLivesDialogController().Init(true);
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
    mGameViewControllers.GetSettingsMenuController().Init(SlidingMenuAnimation::UpdateFade::No,
                                                          mTutorial.IsGestureControlsAllowed());
}

void GameController::GoToPausedStateLevelGoalDialog() {
    mPausedState = PausedState::LevelGoalDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::LevelGoalDialog);
    
    auto levelInfo {LevelLoader::LoadInfo(mLevel->GetId(), mLevelResources)};
    mGameViewControllers.GetLevelGoalDialogController().Init(*levelInfo);
}

void GameController::GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade) {
    mPausedState = PausedState::GameMenu;
    mGameViewControllers.SetActiveController(GameViewControllers::GameMenu);
    auto isUndoMovePossible {mShouldUpdateGameLogic && mGameLogic.IsUndoMovePossible()};
    mGameViewControllers.GetGameMenuController().Init(updateFade, isUndoMovePossible);
    mTutorial.OnPause();
}

void GameController::GoToGameOverStateGameOverDialog() {
    mGameOverState = GameOverState::GameOverDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::GameOverDialog);
    mGameViewControllers.GetGameOverDialogController().Init();
}

void GameController::GoToGameOverStateNoLivesDialog() {
    mGameOverState = GameOverState::NoLivesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::NoLivesDialog);
    mGameViewControllers.GetNoLivesDialogController().Init(false);
}
