#ifndef GameViewControllers_hpp
#define GameViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "GuiViewManager.hpp"
#include "GameHudController.hpp"
#include "GameMenuController.hpp"
#include "GameOverDialogController.hpp"
#include "NoMovesDialogController.hpp"
#include "LevelCompletedDialogController.hpp"
#include "RestartConfirmationDialogController.hpp"
#include "MapConfirmationDialogController.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LevelGoalDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserData;
    class GameScene;
    class Settings;
    class PieceResources;
    
    class GameViewControllers {
    public:
        enum Controller {
            GameHud,
            GameMenu,
            GameOverDialog,
            NoMovesDialog,
            LevelCompletedDialog,
            SettingsMenu,
            NoLivesDialog,
            LevelGoalDialog,
            RestartConfirmationDialog,
            MapConfirmationDialog,
            None
        };
        
        GameViewControllers(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const UserData& userData,
                            Settings& settings,
                            PieceResources& pieceResources);
        
        void Init(GameScene& scene);
        void SetActiveController(Controller controller);
        
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

        LevelGoalDialogController& GetLevelGoalDialogController() {
            return mLevelGoalDialogController;
        }

    private:
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        GameHudController mGameHudController;
        GameMenuController mGameMenuController;
        GameOverDialogController mGameOverDialogController;
        NoMovesDialogController mNoMovesDialogController;
        LevelCompletedDialogController mLevelCompletedDialogController;
        RestartConfirmationDialogController mRestartConfirmationDialogController;
        MapConfirmationDialogController mMapConfirmationDialogController;
        SettingsMenuController mSettingsMenuController;
        NoLivesDialogController mNoLivesDialogController;
        LevelGoalDialogController mLevelGoalDialogController;
    };
}

#endif
