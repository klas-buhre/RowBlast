#ifndef CommonViewControllers_hpp
#define CommonViewControllers_hpp

// Game includes.
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    class Settings;
    
    class CommonViewControllers {
    public:
        CommonViewControllers(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              const UserData& userData,
                              Settings& settings);
        
        const SettingsMenuController& GetSettingsMenuController() const {
            return mSettingsMenuController;
        }
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
        }

        const NoLivesDialogController& GetNoLivesDialogController() const {
            return mNoLivesDialogController;
        }
        
        NoLivesDialogController& GetNoLivesDialogController() {
            return mNoLivesDialogController;
        }
        
    private:
        SettingsMenuController mSettingsMenuController;
        NoLivesDialogController mNoLivesDialogController;
    };
}

#endif
