#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "MapHudController.hpp"
#include "LevelGoalDialogController.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LivesDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class UserServices;
    class Settings;
    class PieceResources;
    
    class MapViewControllers {
    public:
        enum Controller {
            MapHud,
            LevelGoalDialog,
            NoLivesDialog,
            LivesDialog,
            SettingsMenu,
            None
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           const CommonResources& commonResources,
                           const UserServices& userServices,
                           Settings& settings,
                           PieceResources& pieceResources);

        void Init(Pht::FadeEffect& storeFadeEffect);
        void SetActiveController(Controller controller);
    
        MapHudController& GetMapHudController() {
            return mMapHudController;
        }
        
        LevelGoalDialogController& GetLevelGoalDialogController() {
            return mLevelGoalDialogController;
        }

        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }

        LivesDialogController& GetLivesDialogController() {
            return mLivesDialogController;
        }

        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }

    private:
        MapScene& mScene;
        Pht::FadeEffect mFadeEffect;
        Pht::GuiViewManager mViewManager;
        MapHudController mMapHudController;
        LevelGoalDialogController mLevelGoalDialogController;
        NoLivesDialogController mNoLivesDialogController;
        LivesDialogController mLivesDialogController;
        SettingsMenuController mSettingsMenuController;
    };
}

#endif
