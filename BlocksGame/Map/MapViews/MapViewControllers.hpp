#ifndef MapViewControllers_hpp
#define MapViewControllers_hpp

// Game includes.
#include "SettingsButtonController.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class SettingsMenuController;
    class NoLivesDialogController;
    class CommonViewControllers;
    
    class MapViewControllers {
    public:
        enum Controller {
            SettingsButton,
            NoLivesDialog,
            SettingsMenu
        };
        
        MapViewControllers(Pht::IEngine& engine, CommonViewControllers& commonViewControllers);
        
        Controller GetActiveController() const {
            return mActiveController;
        }
        
        void SetActiveController(Controller controller) {
            mActiveController = controller;
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
        Controller mActiveController {SettingsButton};
        SettingsButtonController mSettingsButtonController;
        NoLivesDialogController& mNoLivesDialogController;
        SettingsMenuController& mSettingsMenuController;
    };
}

#endif
