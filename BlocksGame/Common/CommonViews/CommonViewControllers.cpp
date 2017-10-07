#include "CommonViewControllers.hpp"

using namespace BlocksGame;

CommonViewControllers::CommonViewControllers(Pht::IEngine& engine,
                                             const CommonResources& commonResources,
                                             const UserData& userData,
                                             Settings& settings) :
    mSettingsMenuController {engine, commonResources, settings},
    mNoLivesDialogController {engine, commonResources, userData} {}
