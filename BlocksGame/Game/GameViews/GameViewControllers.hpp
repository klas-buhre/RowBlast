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

        const GameHudController& GetGameHudController() const {
            return mGameHudController;
        }
        
        GameHudController& GetGameHudController() {
            return mGameHudController;
        }

        const GameMenuController& GetGameMenuController() const {
            return mGameMenuController;
        }
        
        GameMenuController& GetGameMenuController() {
            return mGameMenuController;
        }
        
        const GameOverDialogController& GetGameOverDialogController() const {
            return mGameOverDialogController;
        }
        
        GameOverDialogController& GetGameOverDialogController() {
            return mGameOverDialogController;
        }
        
        const NoMovesDialogController& GetNoMovesDialogController() const {
            return mNoMovesDialogController;
        }
        
        NoMovesDialogController& GetNoMovesDialogController() {
            return mNoMovesDialogController;
        }

        const LevelCompletedDialogController& GetLevelCompletedDialogController() const {
            return mLevelCompletedDialogController;
        }
        
        LevelCompletedDialogController& GetLevelCompletedDialogController() {
            return mLevelCompletedDialogController;
        }
        
        const RestartConfirmationDialogController& GetRestartConfirmationDialogController() const {
            return mRestartConfirmationDialogController;
        }
        
        RestartConfirmationDialogController& GetRestartConfirmationDialogController() {
            return mRestartConfirmationDialogController;
        }

        const MapConfirmationDialogController& GetMapConfirmationDialogController() const {
            return mMapConfirmationDialogController;
        }
        
        MapConfirmationDialogController& GetMapConfirmationDialogController() {
            return mMapConfirmationDialogController;
        }
        
        const SettingsMenuController& GetSettingsMenuController() const {
            return mSettingsMenuController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }

        const NoLivesDialogController& GetNoLivesDialogController() const {
            return mNoLivesDialogController;
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
