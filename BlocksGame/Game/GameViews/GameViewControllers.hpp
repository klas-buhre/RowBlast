#ifndef GameViewControllers_hpp
#define GameViewControllers_hpp

#include "GameHudController.hpp"
#include "GameMenuController.hpp"
#include "GameOverDialogController.hpp"
#include "NoMovesDialogController.hpp"
#include "LevelCompletedDialogController.hpp"
#include "RestartConfirmationDialogController.hpp"
#include "MapConfirmationDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    class CommonViewControllers;
    class SettingsMenuController;
    class NoLivesDialogController;
    
    class GameViewControllers {
    public:
        enum Controller {
            None,
            GameHud,
            GameMenu,
            GameOverDialog,
            NoMovesDialog,
            LevelCompletedDialog,
            SettingsMenu,
            NoLivesDialog,
            RestartConfirmationDialog,
            MapConfirmationDialog
        };
        
        GameViewControllers(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const UserData& userData,
                            CommonViewControllers& commonViewControllers);
        
        Controller GetActiveController() const {
            return mActiveController;
        }
        
        void SetActiveController(Controller controller) {
            mActiveController = controller;
        }
        
        GameHudController& GetGameHudController() {
            return mGameHudController;
        }

        GameMenuController& GetGameMenuController() {
            return mGameMenuController;
        }
        
        GameOverDialogController& GetGameOverDialogController() {
            return mGameOverDialogController;
        }
        
        NoMovesDialogController& GetNoMovesDialogController() {
            return mNoMovesDialogController;
        }

        LevelCompletedDialogController& GetLevelCompletedDialogController() {
            return mLevelCompletedDialogController;
        }
        
        RestartConfirmationDialogController& GetRestartConfirmationDialogController() {
            return mRestartConfirmationDialogController;
        }

        MapConfirmationDialogController& GetMapConfirmationDialogController() {
            return mMapConfirmationDialogController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }
        
        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }
        
    private:
        Controller mActiveController {None};
        GameHudController mGameHudController;
        GameMenuController mGameMenuController;
        GameOverDialogController mGameOverDialogController;
        NoMovesDialogController mNoMovesDialogController;
        LevelCompletedDialogController mLevelCompletedDialogController;
        RestartConfirmationDialogController mRestartConfirmationDialogController;
        MapConfirmationDialogController mMapConfirmationDialogController;
        SettingsMenuController& mSettingsMenuController;
        NoLivesDialogController& mNoLivesDialogController;
    };
}

#endif
