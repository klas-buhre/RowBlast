#include "MapViewControllers.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.6f};
    constexpr auto fadeTime {0.3f};
}

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       const CommonResources& commonResources,
                                       const UserData& userData,
                                       Settings& settings) :
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mSettingsButtonController {engine},
    mNoLivesDialogController {engine, commonResources, userData},
    mSettingsMenuController {engine, commonResources, settings} {
    
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
