#include "MapViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.3f};
    constexpr auto fadeTime {0.3f};
}

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       const CommonResources& commonResources,
                                       const UserData& userData,
                                       Settings& settings,
                                       PieceResources& pieceResources) :
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mSettingsButtonController {engine},
    mLevelStartDialogController {engine, commonResources, pieceResources},
    mNoLivesDialogController {engine, commonResources, userData, PotentiallyZoomedScreen::No},
    mSettingsMenuController {engine, commonResources, settings, PotentiallyZoomedScreen::No} {
    
    mViewManager.AddView(mSettingsButtonController.GetView());
    mViewManager.AddView(mLevelStartDialogController.GetView());
    mViewManager.AddView(mNoLivesDialogController.GetView());
    mViewManager.AddView(mSettingsMenuController.GetView());
}

void MapViewControllers::Init() {
    mFadeEffect.Reset();

    mLevelStartDialogController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());
    
    mViewManager.Init(uiViewContainer);
    SetActiveController(SettingsButton);
}

void MapViewControllers::SetActiveController(Controller controller) {
    mViewManager.ActivateView(static_cast<int>(controller));
}
