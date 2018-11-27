#ifndef StoreController_hpp
#define StoreController_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "GuiViewManager.hpp"
#include "StoreMenuController.hpp"
#include "PurchaseSuccessfulDialogController.hpp"
#include "PurchaseFailedDialogController.hpp"
#include "PurchaseCanceledDialogController.hpp"
#include "UserServices.hpp"
#include "SpinningWheelEffect.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class StoreController {
    public:
        enum class SceneId {
            Map,
            Game
        };
        
        enum class Result {
            None,
            Done
        };
        
        enum class TriggerProduct {
            Coins,
            Moves,
            Lives
        };
        
        StoreController(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        UserServices& userServices,
                        SceneId sceneId);
        
        void Init(Pht::SceneObject& parentObject);
        void StartStore(TriggerProduct triggerProduct);
        Result Update();

    private:
        enum class ViewController {
            StoreMenu,
            PurchaseSuccessfulDialog,
            PurchaseFailedDialog,
            PurchaseCanceledDialog,
            None
        };

        Result UpdateStoreMenu();
        void StartPurchase(ProductId productId);
        void OnPurchaseFailed(PurchaseFailureReason purchaseFailureReason);
        Result UpdatePurchaseSuccessfulDialog();
        void UpdatePurchaseFailedDialog();
        void UpdatePurchaseCanceledDialog();
        void SetActiveViewController(ViewController viewController);
        void GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade,
                                SlidingMenuAnimation::SlideDirection slideDirection);
        void GoToPurchaseSuccessfulDialogState(const GoldCoinProduct& product);
        void GoToPurchaseFailedDialogState();
        void GoToPurchaseCanceledDialogState();
        void GoToIdleState();
        void GoToPurchasePendingState();
        
        enum class State {
            StoreMenu,
            PurchasePending,
            PurchaseSuccessfulDialog,
            PurchaseFailedDialog,
            PurchaseCanceledDialog,
            Idle
        };

        UserServices& mUserServices;
        State mState {State::Idle};
        TriggerProduct mTriggerProduct;
        Pht::FadeEffect mFadeEffect;
        SpinningWheelEffect mSpinningWheelEffect;
        GuiViewManager mViewManager;
        StoreMenuController mStoreMenuController;
        PurchaseSuccessfulDialogController mPurchaseSuccessfulDialogController;
        PurchaseFailedDialogController mPurchaseFailedDialogController;
        PurchaseCanceledDialogController mPurchaseCanceledDialogController;
    };
}

#endif
