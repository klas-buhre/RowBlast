#ifndef GameViewControllers_hpp
#define GameViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "GuiViewManager.hpp"
#include "GameHudController.hpp"
#include "GameMenuController.hpp"
#include "GameOverDialogController.hpp"
#include "OutOfMovesDialogController.hpp"
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
    class UserServices;
    class GameScene;
    class Settings;
    class PieceResources;
    class GameHudRectangles;
    
    class GameViewControllers {
    public:
        enum Controller {
            GameHud,
            GameMenu,
            GameOverDialog,
            OutOfMovesDialog,
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
                            const UserServices& userServices,
                            Settings& settings,
                            const PieceResources& pieceResources,
                            const GameHudRectangles& hudRectangles);
        
        void Init(GameScene& scene, Pht::FadeEffect& storeFadeEffect);
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
        
        OutOfMovesDialogController& GetOutOfMovesDialogController() {
            return mOutOfMovesDialogController;
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
        OutOfMovesDialogController mOutOfMovesDialogController;
        LevelCompletedDialogController mLevelCompletedDialogController;
        RestartConfirmationDialogController mRestartConfirmationDialogController;
        MapConfirmationDialogController mMapConfirmationDialogController;
        SettingsMenuController mSettingsMenuController;
        NoLivesDialogController mNoLivesDialogController;
        LevelGoalDialogController mLevelGoalDialogController;
    };
}

#endif
