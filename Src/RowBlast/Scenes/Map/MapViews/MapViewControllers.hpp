#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "MapHudController.hpp"
#include "LevelGoalDialogController.hpp"
#include "OptionsMenuController.hpp"
#include "AboutMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LivesDialogController.hpp"
#include "HowToPlayDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class UserServices;
    class PieceResources;
    class LevelResources;
    
    class MapViewControllers {
    public:
        enum Controller {
            MapHud,
            LevelGoalDialog,
            NoLivesDialog,
            LivesDialog,
            OptionsMenu,
            AboutMenu,
            HowToPlayDialog,
            None
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           const CommonResources& commonResources,
                           UserServices& userServices,
                           const PieceResources& pieceResources,
                           const LevelResources& levelResources);

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
        
        AboutMenuController& GetAboutMenuController() {
            return mAboutMenuController;
        }
        
        HowToPlayDialogController& GetHowToPlayDialogController() {
            return mHowToPlayDialogController;
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
        AboutMenuController mAboutMenuController;
        HowToPlayDialogController mHowToPlayDialogController;
    };
}

#endif
