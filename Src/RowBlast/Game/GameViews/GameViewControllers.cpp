#include "GameViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
}

GameViewControllers::GameViewControllers(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const UserServices& userServices,
                                         Settings& settings,
                                         const PieceResources& pieceResources,
                                         const GameHudRectangles& hudRectangles) :
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mGameHudController {engine, commonResources, hudRectangles},
    mGameMenuController {engine, commonResources},
    mGameOverDialogController {engine, commonResources, userServices},
    mOutOfMovesDialogController {engine, commonResources, userServices},
    mLevelCompletedDialogController {engine, commonResources},
    mRestartConfirmationDialogController {engine, commonResources, userServices},
    mMapConfirmationDialogController {engine, commonResources},
    mSettingsMenuController {engine, commonResources, settings, PotentiallyZoomedScreen::Yes},
    mNoLivesDialogController {engine, commonResources, userServices, PotentiallyZoomedScreen::Yes},
    mLevelGoalDialogController {
        engine,
        commonResources,
        pieceResources,
        LevelGoalDialogView::SceneId::Game
    } {
        
    mViewManager.AddView(static_cast<int>(GameHud), mGameHudController.GetView());
    mViewManager.AddView(static_cast<int>(GameMenu), mGameMenuController.GetView());
    mViewManager.AddView(static_cast<int>(GameOverDialog), mGameOverDialogController.GetView());
    mViewManager.AddView(static_cast<int>(OutOfMovesDialog), mOutOfMovesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LevelCompletedDialog), mLevelCompletedDialogController.GetView());
    mViewManager.AddView(static_cast<int>(SettingsMenu), mSettingsMenuController.GetView());
    mViewManager.AddView(static_cast<int>(NoLivesDialog), mNoLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LevelGoalDialog), mLevelGoalDialogController.GetView());
    mViewManager.AddView(static_cast<int>(RestartConfirmationDialog), mRestartConfirmationDialogController.GetView());
    mViewManager.AddView(static_cast<int>(MapConfirmationDialog), mMapConfirmationDialogController.GetView());
    
    mGameMenuController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);
    mRestartConfirmationDialogController.SetFadeEffect(mFadeEffect);
    mMapConfirmationDialogController.SetFadeEffect(mFadeEffect);
    mGameOverDialogController.SetFadeEffect(mFadeEffect);
}

void GameViewControllers::Init(GameScene& scene, Pht::FadeEffect& storeFadeEffect) {
    mFadeEffect.Reset();
    
    auto& uiViewContainer {scene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());
 
    mViewManager.Init(uiViewContainer);
    SetActiveController(None);
    
    mNoLivesDialogController.SetFadeEffect(storeFadeEffect);
    mOutOfMovesDialogController.SetFadeEffect(storeFadeEffect);
    
    mGameMenuController.SetGuiLightProvider(scene);
    mNoLivesDialogController.SetGuiLightProvider(scene);
    mLevelGoalDialogController.SetGuiLightProvider(scene);
}

void GameViewControllers::SetActiveController(Controller controller) {
    if (controller == None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(controller));
    }
}
