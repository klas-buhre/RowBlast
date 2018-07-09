#include "GameViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.6f};
    constexpr auto fadeTime {0.3f};
}

GameViewControllers::GameViewControllers(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const UserData& userData,
                                         Settings& settings,
                                         PieceResources& pieceResources) :
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mGameHudController {engine, commonResources},
    mGameMenuController {engine, commonResources},
    mGameOverDialogController {engine, commonResources, userData},
    mNoMovesDialogController {engine, commonResources},
    mLevelCompletedDialogController {engine, commonResources},
    mRestartConfirmationDialogController {engine, commonResources, userData},
    mMapConfirmationDialogController {engine, commonResources},
    mSettingsMenuController {engine, commonResources, settings, PotentiallyZoomedScreen::Yes},
    mNoLivesDialogController {engine, commonResources, userData, PotentiallyZoomedScreen::Yes},
    mLevelGoalDialogController {
        engine,
        commonResources,
        pieceResources,
        PotentiallyZoomedScreen::Yes
    } {
        
    mViewManager.AddView(static_cast<int>(GameHud), mGameHudController.GetView());
    mViewManager.AddView(static_cast<int>(GameMenu), mGameMenuController.GetView());
    mViewManager.AddView(static_cast<int>(GameOverDialog), mGameOverDialogController.GetView());
    mViewManager.AddView(static_cast<int>(NoMovesDialog), mNoMovesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LevelCompletedDialog), mLevelCompletedDialogController.GetView());
    mViewManager.AddView(static_cast<int>(SettingsMenu), mSettingsMenuController.GetView());
    mViewManager.AddView(static_cast<int>(NoLivesDialog), mNoLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LevelGoalDialog), mLevelGoalDialogController.GetView());
    mViewManager.AddView(static_cast<int>(RestartConfirmationDialog), mRestartConfirmationDialogController.GetView());
    mViewManager.AddView(static_cast<int>(MapConfirmationDialog), mMapConfirmationDialogController.GetView());
}

void GameViewControllers::Init(GameScene& scene) {
    mFadeEffect.Reset();
    
    mGameMenuController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);
    mRestartConfirmationDialogController.SetFadeEffect(mFadeEffect);
    mMapConfirmationDialogController.SetFadeEffect(mFadeEffect);
    
    auto& uiViewContainer {scene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());
 
    mViewManager.Init(uiViewContainer);
    SetActiveController(None);
}

void GameViewControllers::SetActiveController(Controller controller) {
    if (controller == None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(controller));
    }
}
