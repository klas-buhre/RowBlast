#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "MapHudController.hpp"
#include "LevelGoalDialogController.hpp"
#include "OptionsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LivesDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class UserServices;
    class PieceResources;
    
    class MapViewControllers {
    public:
        enum Controller {
            MapHud,
            LevelGoalDialog,
            NoLivesDialog,
            LivesDialog,
            OptionsMenu,
            None
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           const CommonResources& commonResources,
                           UserServices& userServices,
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

        OptionsMenuController& GetOptionsMenuController() {
            return mOptionsMenuController;
        }

    private:
        MapScene& mScene;
        Pht::FadeEffect mFadeEffect;
        Pht::GuiViewManager mViewManager;
        MapHudController mMapHudController;
        LevelGoalDialogController mLevelGoalDialogController;
        NoLivesDialogController mNoLivesDialogController;
        LivesDialogController mLivesDialogController;
        OptionsMenuController mOptionsMenuController;
    };
}

#endif
