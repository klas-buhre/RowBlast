#include "GameViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.6f};
    constexpr auto fadeTime {0.3f};
}

GameViewControllers::GameViewControllers(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const UserData& userData,
                                         Settings& settings) :
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mGameHudController {engine},
    mGameMenuController {engine, commonResources},
    mGameOverDialogController {engine, commonResources, userData},
    mNoMovesDialogController {engine, commonResources},
    mLevelCompletedDialogController {engine, commonResources},
    mRestartConfirmationDialogController {engine, commonResources, userData},
    mMapConfirmationDialogController {engine, commonResources},
    mNoLivesDialogController {engine, commonResources, userData},
    mSettingsMenuController {engine, commonResources, settings} {

    mViewManager.AddView(mGameHudController.GetView());
    mViewManager.AddView(mGameMenuController.GetView());
    mViewManager.AddView(mGameOverDialogController.GetView());
    mViewManager.AddView(mNoMovesDialogController.GetView());
    mViewManager.AddView(mLevelCompletedDialogController.GetView());
    mViewManager.AddView(mSettingsMenuController.GetView());
    mViewManager.AddView(mNoLivesDialogController.GetView());
    mViewManager.AddView(mRestartConfirmationDialogController.GetView());
    mViewManager.AddView(mMapConfirmationDialogController.GetView());
}

void GameViewControllers::Init(GameScene& scene) {
    mFadeEffect.Reset();
    
    mGameMenuController.SetFadeEffect(mFadeEffect);
    mGameOverDialogController.SetFadeEffect(mFadeEffect);
    mNoMovesDialogController.SetFadeEffect(mFadeEffect);
    mLevelCompletedDialogController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);
    mNoLivesDialogController.SetFadeEffect(mFadeEffect);
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
