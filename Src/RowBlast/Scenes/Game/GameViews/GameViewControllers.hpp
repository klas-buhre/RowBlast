#ifndef GameViewControllers_hpp
#define GameViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "GameHudController.hpp"
#include "GameMenuController.hpp"
#include "GameOverDialogController.hpp"
#include "OutOfMovesContinueDialogController.hpp"
#include "OutOfMovesRetryDialogController.hpp"
#include "LevelCompletedDialogController.hpp"
#include "RestartConfirmationDialogController.hpp"
#include "MapConfirmationDialogController.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LevelGoalDialogController.hpp"
#include "HowToPlayDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserServices;
    class GameScene;
    class PieceResources;
    class LevelResources;
    
    class GameViewControllers {
    public:
        enum Controller {
            GameHud,
            GameMenu,
            GameOverDialog,
            OutOfMovesContinueDialog,
            OutOfMovesRetryDialog,
            LevelCompletedDialog,
            SettingsMenu,
            NoLivesDialog,
            LevelGoalDialog,
            RestartConfirmationDialog,
            MapConfirmationDialog,
            HowToPlayDialog,
            None
        };
        
        GameViewControllers(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            UserServices& userServices,
                            const PieceResources& pieceResources,
                            const LevelResources& levelResources);
        
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
        
        OutOfMovesContinueDialogController& GetOutOfMovesContinueDialogController() {
            return mOutOfMovesContinueDialogController;
        }

        OutOfMovesRetryDialogController& GetOutOfMovesRetryDialogController() {
            return mOutOfMovesRetryDialogController;
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
        
        HowToPlayDialogController& GetHowToPlayDialogController() {
            return mHowToPlayDialogController;
        }

    private:
        Pht::FadeEffect mFadeEffect;
        Pht::GuiViewManager mViewManager;
        GameHudController mGameHudController;
        GameMenuController mGameMenuController;
        GameOverDialogController mGameOverDialogController;
        OutOfMovesContinueDialogController mOutOfMovesContinueDialogController;
        OutOfMovesRetryDialogController mOutOfMovesRetryDialogController;
        LevelCompletedDialogController mLevelCompletedDialogController;
        RestartConfirmationDialogController mRestartConfirmationDialogController;
        MapConfirmationDialogController mMapConfirmationDialogController;
        SettingsMenuController mSettingsMenuController;
        NoLivesDialogController mNoLivesDialogController;
        LevelGoalDialogController mLevelGoalDialogController;
        HowToPlayDialogController mHowToPlayDialogController;
    };
}

#endif
