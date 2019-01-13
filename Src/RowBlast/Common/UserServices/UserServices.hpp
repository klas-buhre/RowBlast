#ifndef UserServices_hpp
#define UserServices_hpp

#include "LifeService.hpp"
#include "ProgressService.hpp"
#include "PurchasingService.hpp"
#include "SettingsService.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserServices {
    public:
        UserServices(Pht::IEngine& engine);
        
        void Update();
        void StartLevel(int levelId);
        void CompleteLevel(int levelId, int numStars);
        
        const PurchasingService& GetPurchasingService() const {
            return mPurchasingService;
        }

        PurchasingService& GetPurchasingService() {
            return mPurchasingService;
        }
        
        const LifeService& GetLifeService() const {
            return mLifeService;
        }

        LifeService& GetLifeService() {
            return mLifeService;
        }

        const ProgressService& GetProgressService() const {
            return mProgressService;
        }

        ProgressService& GetProgressService() {
            return mProgressService;
        }
        
        const SettingsService& GetSettingsService() const {
            return mSettingsService;
        }

        SettingsService& GetSettingsService() {
            return mSettingsService;
        }

    private:
        PurchasingService mPurchasingService;
        LifeService mLifeService;
        ProgressService mProgressService;
        SettingsService mSettingsService;
    };
}

#endif
