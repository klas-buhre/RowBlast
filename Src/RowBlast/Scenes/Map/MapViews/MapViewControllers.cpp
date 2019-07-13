#include "MapViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade = 0.5f;
    constexpr auto fadeTime = 0.3f;
}

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       const CommonResources& commonResources,
                                       UserServices& userServices,
                                       const PieceResources& pieceResources,
                                       const LevelResources& levelResources) :
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
    mOptionsMenuController {engine, commonResources, userServices},
    mAboutMenuController {engine, commonResources},
    mHowToPlayDialogController {
        engine,
        commonResources,
        pieceResources,
        levelResources,
        HowToPlayDialogView::SceneId::Map
    },
    mNoMoreLevelsDialogController {engine, commonResources} {
    
    mViewManager.AddView(static_cast<int>(MapHud), mMapHudController.GetView());
    mViewManager.AddView(static_cast<int>(LevelGoalDialog), mLevelGoalDialogController.GetView());
    mViewManager.AddView(static_cast<int>(NoLivesDialog), mNoLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(LivesDialog), mLivesDialogController.GetView());
    mViewManager.AddView(static_cast<int>(OptionsMenu), mOptionsMenuController.GetView());
    mViewManager.AddView(static_cast<int>(AboutMenu), mAboutMenuController.GetView());
    mViewManager.AddView(static_cast<int>(HowToPlayDialog), mHowToPlayDialogController.GetView());
    mViewManager.AddView(static_cast<int>(NoMoreLevelsDialog), mNoMoreLevelsDialogController.GetView());
    
    mLevelGoalDialogController.SetFadeEffect(mFadeEffect);
    mLivesDialogController.SetFadeEffect(mFadeEffect);
    mOptionsMenuController.SetFadeEffect(mFadeEffect);
    mAboutMenuController.SetFadeEffect(mFadeEffect);
    mHowToPlayDialogController.SetFadeEffect(mFadeEffect);
    mNoMoreLevelsDialogController.SetFadeEffect(mFadeEffect);
    
    mLevelGoalDialogController.SetGuiLightProvider(scene);
    mNoLivesDialogController.SetGuiLightProvider(scene);
    mHowToPlayDialogController.SetGuiLightProvider(scene);
}

void MapViewControllers::Init(Pht::FadeEffect& storeFadeEffect) {
    mFadeEffect.Reset();
    
    mNoLivesDialogController.SetFadeEffect(storeFadeEffect);

    auto& uiViewContainer = mScene.GetUiViewsContainer();
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
