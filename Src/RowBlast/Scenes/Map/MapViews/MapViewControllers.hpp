#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "MapHudController.hpp"
#include "LevelGoalDialogController.hpp"
#include "OptionsMenuController.hpp"
#include "SettingsMenuController.hpp"
#include "AboutMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "LivesDialogController.hpp"
#include "HowToPlayDialogController.hpp"
#include "SwipeControlsHintDialogController.hpp"
#include "NoMoreLevelsDialogController.hpp"

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
            SettingsMenu,
            AboutMenu,
            HowToPlayDialog,
            SwipeControlsHintDialog,
            NoMoreLevelsDialog,
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
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }
        
        AboutMenuController& GetAboutMenuController() {
            return mAboutMenuController;
        }
        
        HowToPlayDialogController& GetHowToPlayDialogController() {
            return mHowToPlayDialogController;
        }

        SwipeControlsHintDialogController& GetSwipeControlsHintDialogController() {
            return mSwipeControlsHintDialogController;
        }

        NoMoreLevelsDialogController& GetNoMoreLevelsDialogController() {
            return mNoMoreLevelsDialogController;
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
        SettingsMenuController mSettingsMenuController;
        AboutMenuController mAboutMenuController;
        HowToPlayDialogController mHowToPlayDialogController;
        SwipeControlsHintDialogController mSwipeControlsHintDialogController;
        NoMoreLevelsDialogController mNoMoreLevelsDialogController;
    };
}

#endif
