#include "GameController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"

// Game includes.
#include "LevelLoader.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserServices.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    bool ShouldUpdateGameLogic(CollapsingFieldAnimationSystem::State fieldAnimationState,
                               FallingPieceAnimation::State fallingPieceAnimationState,
                               EffectManager::State effectsState,
                               ScrollController::State scrollState) {
        return (fieldAnimationState == CollapsingFieldAnimationSystem::State::Inactive ||
                fieldAnimationState == CollapsingFieldAnimationSystem::State::BlocksBouncing) &&
               fallingPieceAnimationState == FallingPieceAnimation::State::Inactive &&
               effectsState != EffectManager::State::OngoingEffects &&
               scrollState != ScrollController::State::Scrolling &&
               scrollState != ScrollController::State::LevelOverviewScroll &&
               scrollState != ScrollController::State::BeforeLevelOverviewScroll;
    }
}

GameController::GameController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               UserServices& userServices) :
    mEngine {engine},
    mUserServices {userServices},
    mPieceResources {engine, commonResources},
    mGhostPieceBlocks {engine, commonResources},
    mLevelResources {engine, commonResources},
    mGameHudResources {engine, commonResources},
    mGameViewControllers {engine, commonResources, mUserServices, mPieceResources, mLevelResources},
    mField {},
    mCollapsingFieldAnimation {mField},
    mFlashingBlocksAnimation {mField, mPieceResources},
    mBondsAnimationSystem {mField},
    mScrollController {engine, mField},
    mCameraShake {Pht::CameraShake::ShakeKind::PerlinNoise},
    mScene {
        engine,
        mScrollController,
        commonResources,
        mLevelResources,
        mPieceResources,
        mField,
        mGameViewControllers.GetGameHudController(),
        mCameraShake,
        mGameHudResources
    },
    mStoreController {
        engine,
        commonResources,
        userServices,
        mScene,
        StoreController::SceneId::Game
    },
    mEffectManager {engine, mScene, mCameraShake},
    mPieceDropParticleEffect {engine, mScene},
    mPieceTrailParticleEffect {engine, mScene},
    mPiecePathSystem {engine, mScene},
    mBlastArea {engine, mScene, commonResources},
    mShield {engine, mScene, mScrollController},
    mFieldBottomGlow {engine, mScene, mScrollController},
    mSlidingText {engine, mScene, commonResources, mLevelResources},
    mScoreTexts {mScene, commonResources, ScoreTexts::SceneId::Game},
    mMediumText {engine, mScene, commonResources},
    mFallingPieceScaleAnimation {mScene},
    mTutorial {
        engine,
        mScene,
        commonResources,
        mPieceResources,
        mGhostPieceBlocks,
        mLevelResources,
        mBlastArea,
        userServices
    },
    mGameLogic {
        engine,
        mField,
        mScrollController,
        mScene,
        mEffectManager,
        mFlyingBlocksSystem,
        mFlashingBlocksAnimation,
        mCollapsingFieldAnimation,
        mPieceDropParticleEffect,
        mPieceTrailParticleEffect,
        mPiecePathSystem,
        mBlastArea,
        mFallingPieceScaleAnimation,
        mShield,
        mScoreTexts,
        mMediumText,
        mGameViewControllers.GetGameHudController(),
        mTutorial,
        userServices.GetSettingsService()
    },
    mFallingPieceAnimation {mGameLogic.GetFallingPieceAnimation()},
    mDraggedPieceAnimation {mGameLogic.GetDraggedPieceAnimation()},
    mActivePreviewPieceAnimation {mScene, mGameLogic},
    mPreviewPiecesAnimation {mScene, mGameLogic, mActivePreviewPieceAnimation},
    mPreviewPiecesRotationAnimation {mScene, mGameLogic},
    mFewMovesAlertAnimation {engine, mScene, mGameLogic},
    mAddingMovesAnimation {engine, mScene},
    mBlueprintSlotsFilledAnimation {mField, mScene, mLevelResources},
    mBombsAnimation {mScene, mPieceResources, mLevelResources},
    mAsteroidAnimation {},
    mFlyingBlocksSystem {mScene, mLevelResources, mPieceResources, mBombsAnimation},
    mFieldSceneSystem {
        mScene,
        mField,
        mGameLogic,
        mScrollController,
        mBombsAnimation,
        mAsteroidAnimation,
        mFallingPieceAnimation,
        mDraggedPieceAnimation,
        mPieceResources,
        mGhostPieceBlocks,
        mLevelResources
    },
    mLevelCompletedController {
        engine,
        mScene,
        mGameViewControllers,
        mSlidingText,
        mMediumText,
        mGameLogic,
        mUserServices,
        commonResources,
        mCameraShake,
        mField,
        mFlyingBlocksSystem
    } {}

void GameController::Init(int levelId) {
    mLevel = LevelLoader::Load(levelId, mLevelResources);

    mField.Init(*mLevel);
    mScrollController.Init(mLevel->GetObjective());
    mScene.Init(*mLevel, mGameLogic);
    mTutorial.Init(*mLevel);
    mGameLogic.Init(*mLevel);
    mStoreController.Init(mScene.GetUiViewsContainer());
    mBlueprintSlotsFilledAnimation.Init();
    mPieceDropParticleEffect.Init();
    mPieceTrailParticleEffect.Init();
    mPiecePathSystem.Init(*mLevel);
    mBlastArea.Init();
    mShield.Init(*mLevel);
    mFieldBottomGlow.Init();
    mEffectManager.Init();
    mCameraShake.Init();
    mPreviewPiecesAnimation.Init();
    mActivePreviewPieceAnimation.Init();
    mPreviewPiecesRotationAnimation.Init();
    mFewMovesAlertAnimation.Init();
    mAddingMovesAnimation.Init();
    mFlyingBlocksSystem.Init();
    mFlashingBlocksAnimation.Init();
    mFallingPieceAnimation.Init();
    mDraggedPieceAnimation.Init();
    mSlidingText.Init();
    mScoreTexts.Init();
    mMediumText.Init();
    mBombsAnimation.Init();
    mAsteroidAnimation.Init();
    mFallingPieceScaleAnimation.Init();
    mGameViewControllers.Init(mScene, mStoreController.GetFadeEffect());
    mLevelCompletedController.Init(*mLevel);
    
    mState = GameState::LevelIntro;
    mLevelIntroState = LevelIntroState::Overview;
    mIsInBetweenMoves = false;
    mUndoMovePending = false;
    
    mUserServices.StartLevel(levelId);
}

GameController::Command GameController::Update() {
    auto command = Command::None;
    
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
    
    return command;
}

GameController::Command GameController::UpdateGame() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    auto scrollState = mScrollController.Update();
    auto fieldAnimationState = mCollapsingFieldAnimation.Update(dt);
    mBlueprintSlotsFilledAnimation.Update(dt);
    auto effectsState = mEffectManager.GetState();
    auto fallingPieceAnimationState = mFallingPieceAnimation.Update(dt);
    
    if (mState == GameState::Playing) {
        auto shouldUpdateGameLogic =
            ShouldUpdateGameLogic(fieldAnimationState,
                                  fallingPieceAnimationState,
                                  effectsState,
                                  scrollState);
        auto isInBetweenMoves = !shouldUpdateGameLogic;
        mIsInBetweenMoves = isInBetweenMoves;
        
        auto result = mGameLogic.Update(shouldUpdateGameLogic, ShouldUndoMove(isInBetweenMoves));
        if (result != GameLogic::Result::None) {
            ChangeGameState(result);
        }
    }
    
    auto command = UpdateSubState();
    
    mFlashingBlocksAnimation.Update(dt);
    mBondsAnimationSystem.Update(dt);
    mPieceDropParticleEffect.Update(dt);
    mPieceTrailParticleEffect.Update(dt);
    mPiecePathSystem.Update(dt);
    mShield.Update(dt);
    mFieldBottomGlow.Update(dt);
    mEffectManager.Update(dt);
    mCameraShake.Update(dt);
    mFlyingBlocksSystem.Update(dt);
    mDraggedPieceAnimation.Update();
    mFallingPieceScaleAnimation.Update(dt);
    mBlastArea.Update(dt);
    mScene.Update();
    mPreviewPiecesAnimation.Update(dt);
    mActivePreviewPieceAnimation.Update(dt);
    mPreviewPiecesRotationAnimation.Update(dt);
    mFewMovesAlertAnimation.Update(dt);
    mAddingMovesAnimation.Update(dt);
    mBombsAnimation.Update(dt);
    mScoreTexts.Update(dt);
    mMediumText.Update(dt);
    mTutorial.Update();
    
    mFieldSceneSystem.Update();
    mField.OnEndOfFrame();
    
    mAsteroidAnimation.Update(dt);

    return command;
}

bool GameController::ShouldUndoMove(bool isInBetweenMoves) {
    auto shouldUndoMove = mUndoMovePending && !isInBetweenMoves;
    if (shouldUndoMove) {
        mUndoMovePending = false;
    }
    
    return shouldUndoMove;
}

void GameController::ChangeGameState(GameLogic::Result gameLogicResult) {
    switch (gameLogicResult) {
        case GameLogic::Result::Pause:
            mState = GameState::Paused;
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::Yes,
                                    SlidingMenuAnimation::SlideDirection::Left);
            break;
        case GameLogic::Result::OutOfMoves:
            mState = GameState::OutOfMoves;
            GoToOutOfMovesStateOutOfMovesAnimation();
            break;
        case GameLogic::Result::GameOver:
            mState = GameState::GameOver;
            GoToGameOverStateGameOverDialog();
            mUserServices.FailLevel(mLevel->GetId());
            break;
        case GameLogic::Result::LevelCompleted:
            mState = GameState::LevelCompleted;
            mLevelCompletedController.Start();
            break;
        default:
            break;
    }
}

GameController::Command GameController::UpdateSubState() {
    auto command = Command::None;
    
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
            command = UpdateInOutOfMovesState();
            break;
        case GameState::GameOver:
            command = UpdateInGameOverState();
            break;
        default:
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInPausedState() {
    auto command = Command::None;
    
    mUserServices.Update();
    
    switch (mPausedState) {
        case PausedState::GameMenu:
            UpdateGameMenu();
            break;
        case PausedState::HowToPlayDialog:
            UpdateHowToPlayDialog();
            break;
        case PausedState::LevelInfoDialog:
            UpdateLevelInfoDialog();
            break;
        case PausedState::SettingsMenu:
            UpdateSettingsMenu();
            break;
        case PausedState::NoLivesDialog:
            command = UpdateInPausedStateNoLivesDialog();
            break;
        case PausedState::RestartConfirmationDialog:
            command = UpdateRestartConfirmationDialog();
            break;
        case PausedState::MapConfirmationDialog:
            command = UpdateMapConfirmationDialog();
            break;
        case PausedState::Store:
            command = UpdateInPausedStateStore();
            break;
    }
    
    mScene.UpdateLightAnimation();

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
            if (!mIsInBetweenMoves) {
                mGameLogic.UndoMove();
                mFieldSceneSystem.Update();
                mField.OnEndOfFrame();
            }
            break;
        case GameMenuController::Result::ResumeGameAfterUndo:
            if (mIsInBetweenMoves) {
                mUndoMovePending = true;
                mMediumText.StartWillUndoMessage();
            } else {
                mMediumText.StartUndoingMessage();
            }
            GoToPlayingState();
            break;
        case GameMenuController::Result::GoToHowToPlayDialog:
            GoToPausedStateHowToPlayDialog();
            break;
        case GameMenuController::Result::GoToLevelInfoDialog:
            GoToPausedStateLevelInfoDialog();
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

void GameController::UpdateHowToPlayDialog() {
    switch (mGameViewControllers.GetHowToPlayDialogController().Update()) {
        case HowToPlayDialogController::Result::None:
            break;
        case HowToPlayDialogController::Result::Close:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No,
                                    SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

void GameController::UpdateLevelInfoDialog() {
    switch (mGameViewControllers.GetLevelGoalDialogController().Update()) {
        case LevelGoalDialogController::Result::None:
            break;
        case LevelGoalDialogController::Result::Play:
            assert(!"Unexpected result");
            break;
        case LevelGoalDialogController::Result::Close:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No,
                                    SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

void GameController::UpdateSettingsMenu() {
    switch (mGameViewControllers.GetSettingsMenuController().Update()) {
        case SettingsMenuController::Result::None:
            break;
        case SettingsMenuController::Result::GoBack:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No,
                                    SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

GameController::Command GameController::UpdateInPausedStateNoLivesDialog() {
    auto command = Command::None;

    switch (mGameViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                command = Command::RestartLevel;
            } else {
                GoToPausedStateStore();
            }
            break;
        case NoLivesDialogController::Result::Close:
            GoToPlayingState();
            break;
    }
    
    return command;
}

void GameController::RefillLives() {
    mUserServices.GetPurchasingService().WithdrawCoins(CoinWithdrawReason::RefillLives);
    mUserServices.GetLifeService().RefillLives();
    
    Pht::CustomAnalyticsEvent analyticsEvent {
        "Purchases:BoughtLives:Level" + std::to_string(mLevel->GetId())
    };
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}

GameController::Command GameController::UpdateRestartConfirmationDialog() {
    auto command = Command::None;
    
    switch (mGameViewControllers.GetRestartConfirmationDialogController().Update()) {
        case RestartConfirmationDialogController::Result::None:
            break;
        case RestartConfirmationDialogController::Result::RestartLevel:
            if (mUserServices.GetLifeService().GetNumLives() > 0) {
                command = Command::RestartLevel;
                mUserServices.FailLevel(mLevel->GetId());
            } else {
                GoToPausedStateNoLivesDialog();
            }
            break;
        case RestartConfirmationDialogController::Result::DoNotRestartGame:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No,
                                    SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateMapConfirmationDialog() {
    auto command = Command::None;
    
    switch (mGameViewControllers.GetMapConfirmationDialogController().Update()) {
        case MapConfirmationDialogController::Result::None:
            break;
        case MapConfirmationDialogController::Result::GoToMap: {
            mUserServices.FailLevel(mLevel->GetId());
            command = Command::GoToMap;
            break;
        }
        case MapConfirmationDialogController::Result::DoNotGoToMap:
            GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade::No,
                                    SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInPausedStateStore() {
    auto command = Command::None;

    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                command = Command::RestartLevel;
            } else {
                GoToPausedStateRestartConfirmationDialog();
            }
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
            if (mSlidingText.Update() == SlidingText::State::Inactive) {
                if (mTutorial.OnLevelStart() == Tutorial::Result::TutorialHasFocus) {
                    mState = GameState::TutorialDialog;
                } else {
                    mShield.Start();
                    GoToPlayingState();
                }
            }
            break;
    }
}

void GameController::UpdateTutorialDialogs() {
    if (mTutorial.UpdateDialogs() == Tutorial::Result::Play) {
        GoToPlayingState();
        mShield.Start();
    }
}

void GameController::StartLevelObjectiveAnimation() {
    mLevelIntroState = LevelIntroState::ObjectiveAnimation;

    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            mSlidingText.StartClearBlocksMessage(mField.CalculateNumLevelBlocks());
            break;
        case Level::Objective::Build:
            mSlidingText.StartFillSlotsMessage(mField.CalculateNumEmptyBlueprintSlots());
            break;
        case Level::Objective::BringDownTheAsteroid:
            mSlidingText.StartBringDownTheAsteroidMessage();
            break;
    }
}

GameController::Command GameController::UpdateInLevelCompletedState() {
    auto command = Command::None;
    
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

GameController::Command GameController::UpdateInOutOfMovesState() {
    auto command = Command::None;
    
    mUserServices.Update();
    
    switch (mOutOfMovesState) {
        case OutOfMovesState::OutOfMovesAnimation:
            UpdateInOutOfMovesStateOutOfMovesAnimation();
            break;
        case OutOfMovesState::OutOfMovesDialog:
            command = UpdateOutOfMovesDialog();
            break;
        case OutOfMovesState::Store:
            UpdateInOutOfMovesStateStore();
            break;
    }
    
    return command;
}

void GameController::UpdateInOutOfMovesStateOutOfMovesAnimation() {
    if (mSlidingText.Update() == SlidingText::State::Inactive) {
        GoToOutOfMovesStateOutOfMovesDialog(SlidingMenuAnimation::SlideDirection::Left,
                                            SlidingMenuAnimation::UpdateFade::Yes);
    }
}

GameController::Command GameController::UpdateOutOfMovesDialog() {
    auto command = Command::None;
    
    switch (mGameViewControllers.GetOutOfMovesDialogController().Update()) {
        case OutOfMovesDialogController::Result::None:
            break;
        case OutOfMovesDialogController::Result::PlayOn:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::addMovesPriceInCoins)) {
                AddMovesAndGoToPlayingState();
            } else {
                GoToOutOfMovesStateStore();
            }
            break;
        case OutOfMovesDialogController::Result::BackToMap:
            mUserServices.FailLevel(mLevel->GetId(), CalculateProgressInLevelForAnalytics());
            if (mUserServices.GetLifeService().GetNumLives() > 0) {
                command = Command::RestartLevel;
            } else {
                command = Command::GoToMap;
            }
            break;
    }
    
    return command;
}

int GameController::CalculateProgressInLevelForAnalytics() {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
        case Level::Objective::Build:
            return mGameLogic.GetNumObjectsLeftToClear();
        case Level::Objective::BringDownTheAsteroid: {
            auto asteroidRow = mField.CalculateAsteroidRow();
            assert(asteroidRow.HasValue());
            return asteroidRow.GetValue();
        }
    }
}

void GameController::UpdateInOutOfMovesStateStore() {
    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::addMovesPriceInCoins)) {
                AddMovesAndGoToPlayingState();
            } else {
                GoToOutOfMovesStateOutOfMovesDialog(SlidingMenuAnimation::SlideDirection::Right,
                                                    SlidingMenuAnimation::UpdateFade::No);
            }
            break;
    }
}

void GameController::AddMovesAndGoToPlayingState() {
    mUserServices.GetPurchasingService().WithdrawCoins(CoinWithdrawReason::AddMoves);
    mGameLogic.SetMovesLeft(5);
    
    Pht::CustomAnalyticsEvent analyticsEvent {
        "Purchases:BoughtMoves:Level" + std::to_string(mLevel->GetId())
    };
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
    
    mAddingMovesAnimation.Start();
    GoToPlayingState();
}

GameController::Command GameController::UpdateInGameOverState() {
    auto command = Command::None;
    
    mUserServices.Update();
    
    switch (mGameOverState) {
        case GameOverState::GameOverDialog:
            command = UpdateGameOverDialog();
            break;
        case GameOverState::NoLivesDialog:
            command = UpdateInGameOverStateNoLivesDialog();
            break;
        case GameOverState::Store:
            command = UpdateInGameOverStateStore();
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateGameOverDialog() {
    auto command = Command::None;
    
    switch (mGameViewControllers.GetGameOverDialogController().Update()) {
        case GameOverDialogController::Result::None:
            break;
        case GameOverDialogController::Result::Retry:
            if (mUserServices.GetLifeService().GetNumLives() > 0) {
                command = Command::RestartLevel;
            } else {
                GoToGameOverStateNoLivesDialog();
            }
            break;
        case GameOverDialogController::Result::BackToMap:
            command = Command::GoToMap;
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInGameOverStateNoLivesDialog() {
    auto command = Command::None;

    switch (mGameViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                command = Command::RestartLevel;
            } else {
                GoToGameOverStateStore();
            }
            break;
        case NoLivesDialogController::Result::Close:
            command = Command::GoToMap;
            break;
    }
    
    return command;
}

GameController::Command GameController::UpdateInGameOverStateStore() {
    auto command = Command::None;

    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                command = Command::RestartLevel;
            } else {
                GoToGameOverStateNoLivesDialog();
            }
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
    mGameViewControllers.GetNoLivesDialogController().SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                                            NoLivesDialogController::ShouldSlideOut::Yes,
                                                            NoLivesDialogController::ShouldSlideOut::No);
}

void GameController::GoToPausedStateRestartConfirmationDialog() {
    mPausedState = PausedState::RestartConfirmationDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::RestartConfirmationDialog);
    mGameViewControllers.GetRestartConfirmationDialogController().SetUp();
}

void GameController::GoToPausedStateMapConfirmationDialog() {
    mPausedState = PausedState::MapConfirmationDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::MapConfirmationDialog);
    mGameViewControllers.GetMapConfirmationDialogController().SetUp();
}

void GameController::GoToPausedStateStore() {
    mPausedState = PausedState::Store;
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mStoreController.StartStore(TriggerProduct::Lives,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::No,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::No);
}

void GameController::GoToPausedStateSettingsMenu() {
    mPausedState = PausedState::SettingsMenu;
    mGameViewControllers.SetActiveController(GameViewControllers::SettingsMenu);
    mGameViewControllers.GetSettingsMenuController().SetUp(mTutorial.IsGestureControlsAllowed());
}

void GameController::GoToPausedStateLevelInfoDialog() {
    mPausedState = PausedState::LevelInfoDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::LevelGoalDialog);
    
    auto levelInfo = LevelLoader::LoadInfo(mLevel->GetId(), mLevelResources);
    mGameViewControllers.GetLevelGoalDialogController().SetUp(*levelInfo);
}

void GameController::GoToPausedStateHowToPlayDialog() {
    mPausedState = PausedState::HowToPlayDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::HowToPlayDialog);
    mGameViewControllers.GetHowToPlayDialogController().SetUp(0);
}

void GameController::GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade,
                                             SlidingMenuAnimation::SlideDirection slideDirection) {
    mPausedState = PausedState::GameMenu;
    mGameViewControllers.SetActiveController(GameViewControllers::GameMenu);
    
    auto isUndoMovePossible =
        !mUndoMovePending && mTutorial.IsUndoMoveAllowed(mGameLogic.GetMovesUsedIncludingCurrent()) &&
        (mIsInBetweenMoves || mGameLogic.IsUndoMovePossible());
    
    mGameViewControllers.GetGameMenuController().SetUp(updateFade,
                                                       slideDirection,
                                                       isUndoMovePossible);
    mTutorial.OnPause();
}

void GameController::GoToOutOfMovesStateOutOfMovesAnimation() {
    mOutOfMovesState = OutOfMovesState::OutOfMovesAnimation;
    mSlidingText.StartOutOfMovesMessage();
}

void GameController::GoToOutOfMovesStateOutOfMovesDialog(SlidingMenuAnimation::SlideDirection slideDirection,
                                                         SlidingMenuAnimation::UpdateFade updateFade) {
    mOutOfMovesState = OutOfMovesState::OutOfMovesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::OutOfMovesDialog);
    mGameViewControllers.GetOutOfMovesDialogController().SetUp(mScene, slideDirection, updateFade);
}

void GameController::GoToOutOfMovesStateStore() {
    mOutOfMovesState = OutOfMovesState::Store;
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mStoreController.StartStore(TriggerProduct::Moves,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::Yes);
}

void GameController::GoToGameOverStateGameOverDialog() {
    mGameOverState = GameOverState::GameOverDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::GameOverDialog);
    mGameViewControllers.GetGameOverDialogController().SetUp();
}

void GameController::GoToGameOverStateNoLivesDialog() {
    mGameOverState = GameOverState::NoLivesDialog;
    mGameViewControllers.SetActiveController(GameViewControllers::NoLivesDialog);
    mGameViewControllers.GetNoLivesDialogController().SetUp(SlidingMenuAnimation::UpdateFade::No,
                                                            NoLivesDialogController::ShouldSlideOut::No,
                                                            NoLivesDialogController::ShouldSlideOut::No);
}

void GameController::GoToGameOverStateStore() {
    mGameOverState = GameOverState::Store;
    mGameViewControllers.SetActiveController(GameViewControllers::None);
    mStoreController.StartStore(TriggerProduct::Lives,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::No,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::No);
}
