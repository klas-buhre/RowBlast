#include "MapViewControllers.hpp"

// Game includes.
#include "CommonViewControllers.hpp"

using namespace BlocksGame;

MapViewControllers::MapViewControllers(Pht::IEngine& engine,
                                       CommonViewControllers& commonViewControllers) :
    mSettingsButtonController {engine},
    mNoLivesDialogController {commonViewControllers.GetNoLivesDialogController()},
    mSettingsMenuController {commonViewControllers.GetSettingsMenuController()} {}
