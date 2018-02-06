#include "MapViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonViewControllers.hpp"
#include "MapScene.hpp"
#include "UiLayer.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto fade {0.6f};
    constexpr auto fadeTime {0.3f};
}

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       CommonViewControllers& commonViewControllers) :
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mSettingsButtonController {engine},
    mNoLivesDialogController {commonViewControllers.GetNoLivesDialogController()},
    mSettingsMenuController {commonViewControllers.GetSettingsMenuController()} {
    
    mViewManager.AddView(mSettingsButtonController.GetView());
    mViewManager.AddView(mNoLivesDialogController.GetView());
    mViewManager.AddView(mSettingsMenuController.GetView());
}

void MapViewControllers::Init() {
    mFadeEffect.Reset();

    mNoLivesDialogController.SetFadeEffect(mFadeEffect);
    mSettingsMenuController.SetFadeEffect(mFadeEffect);

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());
    
    mViewManager.Init(uiViewContainer);
    SetActiveController(SettingsButton);
}

void MapViewControllers::SetActiveController(Controller controller) {
    mViewManager.ActivateView(static_cast<int>(controller));
}
