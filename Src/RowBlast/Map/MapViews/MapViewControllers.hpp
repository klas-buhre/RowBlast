#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "SettingsButtonController.hpp"
#include "LevelGoalDialogController.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "GuiViewManager.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class UserData;
    class Settings;
    class PieceResources;
    
    class MapViewControllers {
    public:
        enum Controller {
            SettingsButton,
            LevelGoalDialog,
            NoLivesDialog,
            SettingsMenu
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           const CommonResources& commonResources,
                           const UserData& userData,
                           Settings& settings,
                           PieceResources& pieceResources);

        void Init();
        void SetActiveController(Controller controller);
    
        SettingsButtonController& GetSettingsButtonController() {
            return mSettingsButtonController;
        }
        
        LevelGoalDialogController& GetLevelGoalDialogController() {
            return mLevelGoalDialogController;
        }

        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }
        
    private:
        MapScene& mScene;
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        SettingsButtonController mSettingsButtonController;
        LevelGoalDialogController mLevelGoalDialogController;
        NoLivesDialogController mNoLivesDialogController;
        SettingsMenuController mSettingsMenuController;
    };
}

#endif
