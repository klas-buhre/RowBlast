#ifndef CommonViewControllers_hpp
#define CommonViewControllers_hpp

// Game includes.
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserData;
    class Settings;
    
    class CommonViewControllers {
    public:
        CommonViewControllers(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              const UserData& userData,
                              Settings& settings);
        
        SettingsMenuController& GetSettingsMenuController() {
            return mSettingsMenuController;
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
