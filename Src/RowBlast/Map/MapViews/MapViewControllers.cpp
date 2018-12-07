#include "MapViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
}

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       const CommonResources& commonResources,
                                       UserServices& userServices,
                                       PieceResources& pieceResources) :
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mMapHudController {engine, commonResources},
    mLevelGoalDialogController {
        engine,
        commonResources,
        pieceResources,
        LevelGoalDialogView::SceneId::Map
    },
    mNoLivesDialogController {engine, commonResources, userServices, PotentiallyZoomedScreen::No},
    mLivesDialogController {engine, commonResources, userServices, scene},
    mSettingsMenuController {engine, commonResources, userServices, PotentiallyZoomedScreen::No} {
    
    mViewManager.AddView(static_cast<int>(MapHud), mMapHudController.GetView());
    mViewManager.AddView(static_cast<int>(LevelGoalDialog), mLevelGoalDialogController.GetView());
    mViewManager.AddView(static_cast<int>(NoLivesDialog), mNoLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LivesDialog), mLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(SettingsMenu), mSettingsMenuController.GetView());
    
    mLevelGoalDialogController.SetFadeEffect(mFadeEffect);
    mLivesDialogController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);
    
    mLevelGoalDialogController.SetGuiLightProvider(scene);
    mNoLivesDialogController.SetGuiLightProvider(scene);
}

void MapViewControllers::Init(Pht::FadeEffect& storeFadeEffect) {
    mFadeEffect.Reset();
    
    mNoLivesDialogController.SetFadeEffect(storeFadeEffect);

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());
    
    mViewManager.Init(uiViewContainer);
    SetActiveController(MapHud);
}

void MapViewControllers::SetActiveController(Controller controller) {
    if (controller == None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(controller));
    }
}
