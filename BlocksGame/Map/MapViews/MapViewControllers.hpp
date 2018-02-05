#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

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

        Controller GetActiveController() const {
            return mActiveController;
        }

        const SettingsButtonController& GetSettingsButtonController() const {
            return mSettingsButtonController;
        }
        
        SettingsButtonController& GetSettingsButtonController() {
            return mSettingsButtonController;
        }
        
        const NoLivesDialogController& GetNoLivesDialogController() const {
            return mNoLivesDialogController;
        }
        
        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }
        
        const SettingsMenuController& GetSettingsMenuController() const {
            return mSettingsMenuController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }
        
    private:
        MapScene& mScene;
        Controller mActiveController {SettingsButton};
        GuiViewManager mViewManager;
        SettingsButtonController mSettingsButtonController;
        NoLivesDialogController& mNoLivesDialogController;
        SettingsMenuController& mSettingsMenuController;
    };
}

#endif
