#include "MapViewControllers.hpp"

// Game includes.
#include "CommonViewControllers.hpp"
#include "MapScene.hpp"

using namespace BlocksGame;

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       MapScene& scene,
                                       CommonViewControllers& commonViewControllers) :
    mScene {scene},
    mSettingsButtonController {engine},
    mNoLivesDialogController {commonViewControllers.GetNoLivesDialogController()},
    mSettingsMenuController {commonViewControllers.GetSettingsMenuController()} {
    
    mViewManager.AddView(mSettingsButtonController.GetView());
    mViewManager.AddView(mNoLivesDialogController.GetView());
    mViewManager.AddView(mSettingsMenuController.GetView());
}

void MapViewControllers::Init() {
    mViewManager.Init(mScene.GetUiViewsContainer());
}

void MapViewControllers::SetActiveController(Controller controller) {
    mActiveController = controller;
    mViewManager.SetActiveView(static_cast<int>(controller));
}
