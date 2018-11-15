#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "MapHudController.hpp"
#include "LevelGoalDialogController.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "StoreMenuController.hpp"
#include "GuiViewManager.hpp"

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
            SettingsMenu,
            StoreMenu
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           const CommonResources& commonResources,
                           const UserServices& userServices,
                           Settings& settings,
                           PieceResources& pieceResources);

        void Init();
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
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }

        StoreMenuController& GetStoreMenuController() {
            return mStoreMenuController;
        }

    private:
        MapScene& mScene;
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        MapHudController mMapHudController;
        LevelGoalDialogController mLevelGoalDialogController;
        NoLivesDialogController mNoLivesDialogController;
        SettingsMenuController mSettingsMenuController;
        StoreMenuController mStoreMenuController;
    };
}

#endif
