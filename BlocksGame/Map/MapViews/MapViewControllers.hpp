#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "SettingsButtonController.hpp"
#include "GuiViewManager.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class SettingsMenuController;
    class NoLivesDialogController;
    class CommonViewControllers;
    class MapScene;
    
    class MapViewControllers {
    public:
        enum Controller {
            SettingsButton,
            NoLivesDialog,
            SettingsMenu
        };
        
        MapViewControllers(Pht::IEngine& engine,
                           MapScene& scene,
                           CommonViewControllers& commonViewControllers);

        void Init();
        void SetActiveController(Controller controller);
    
        SettingsButtonController& GetSettingsButtonController() {
            return mSettingsButtonController;
        }
        
        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }
        
    private:
        MapScene& mScene;
        Controller mActiveController {SettingsButton};
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        SettingsButtonController mSettingsButtonController;
        NoLivesDialogController& mNoLivesDialogController;
        SettingsMenuController& mSettingsMenuController;
    };
}

#endif
