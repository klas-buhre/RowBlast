#include "GameViewControllers.hpp"

// Game includes.
#include "CommonViewControllers.hpp"

using namespace BlocksGame;

GameViewControllers::GameViewControllers(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const UserData& userData,
                                         CommonViewControllers& commonViewControllers) :
    mGameHudController {engine},
    mGameMenuController {engine, commonResources},
    mGameOverDialogController {engine, commonResources, userData},
    mNoMovesDialogController {engine, commonResources},
    mLevelCompletedDialogController {engine, commonResources},
    mRestartConfirmationDialogController {engine, commonResources},
    mMapConfirmationDialogController {engine, commonResources},
    mSettingsMenuController {commonViewControllers.GetSettingsMenuController()},
    mNoLivesDialogController {commonViewControllers.GetNoLivesDialogController()} {}
