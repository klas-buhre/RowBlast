#ifndef StoreController_hpp
#define StoreController_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "StoreMenuController.hpp"
#include "PurchaseSuccessfulDialogController.hpp"
#include "StoreErrorDialogController.hpp"
#include "PurchaseCanceledDialogController.hpp"
#include "NoInternetAccessDialogController.hpp"
#include "UserServices.hpp"
#include "SpinningWheelEffect.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class IGuiLightProvider;
    
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
                
        StoreController(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        UserServices& userServices,
                        IGuiLightProvider& guiLightProvider,
                        SceneId sceneId);
        
        void Init(Pht::SceneObject& parentObject);
        void StartStore(StoreTrigger storeTrigger,
                        SlidingMenuAnimation::UpdateFade updateFadeOnStart,
                        SlidingMenuAnimation::UpdateFade updateFadeOnClose,
                        SlidingMenuAnimation::UpdateFade updateFadeOnCanAffordTriggerProduct,
                        PurchaseSuccessfulDialogController::ShouldSlideOut slideOutOnCanAffordTriggerProduct);
        Result Update();
        
        Pht::FadeEffect& GetFadeEffect() {
            return mFadeEffect;
        }

    private:
        enum class ViewController {
            StoreMenu,
            PurchaseSuccessfulDialog,
            PurchaseFailedDialog,
            PurchaseCanceledDialog,
            NoInternetAccessDialog,
            CannotContactServerDialog,
            PaymentsDisabledOnDeviceDialog,
            None
        };

        Result UpdateStoreMenu();
        void FetchProducts(SlidingMenuAnimation::UpdateFade updateFadeOnStart);
        void StartPurchase(ProductId productId);
        void OnPurchaseFailed(Pht::PurchaseError error);
        void UpdateInPurchaseSuccessfulDelayState();
        Result UpdatePurchaseSuccessfulDialog();
        void UpdatePaymentsDisabledOnDeviceDialog();
        void UpdatePurchaseFailedDialog();
        void UpdatePurchaseCanceledDialog();
        StoreController::Result UpdateNoInternetAccessDialog();
        void UpdateNoInternetAccessAtPurchaseAttemptDialog();
        StoreController::Result UpdateCannotContactServerDialog();
        void SetActiveViewController(ViewController viewController);
        void GoToNoInternetAccessDialogState(SlidingMenuAnimation::UpdateFade updateFade);
        void GoToNoInternetAccessAtPurchaseAttemptDialogState();
        void GoToCannotContactServerState();
        void GoToFetchingProductsState();
        void GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade,
                                SlidingMenuAnimation::SlideDirection slideDirection);
        void GoToPurchaseSuccessfulDelayState();
        void GoToPurchaseSuccessfulDialogState();
        void GoToPaymentsDisabledOnDeviceDialogState();
        void GoToPurchaseFailedDialogState();
        void GoToPurchaseCanceledDialogState();
        void GoToIdleState();
        void GoToPurchasePendingState();
        
        enum class State {
            FetchingProducts,
            NoInternetAccessDialog,
            CannotContactServerDialog,
            StoreMenu,
            PurchasePending,
            PurchaseSuccessfulDelay,
            PurchaseSuccessfulDialog,
            PurchaseFailedDialog,
            PurchaseCanceledDialog,
            PaymentsDisabledOnDeviceDialog,
            NoInternetAccessAtPurchaseAttemptDialog,
            Idle
        };

        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        State mState {State::Idle};
        StoreTrigger mStoreTrigger;
        GoldCoinProduct mPurchasedProduct;
        Pht::FadeEffect mFadeEffect;
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnClose;
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnCanAffordTriggerProduct;
        PurchaseSuccessfulDialogController::ShouldSlideOut mSlideOutOnCanAffordTriggerProduct;
        SpinningWheelEffect mSpinningWheelEffect;
        Pht::GuiViewManager mViewManager;
        StoreMenuController mStoreMenuController;
        PurchaseSuccessfulDialogController mPurchaseSuccessfulDialogController;
        PurchaseCanceledDialogController mPurchaseCanceledDialogController;
        NoInternetAccessDialogController mNoInternetAccessDialogController;
        StoreErrorDialogController mPurchaseFailedDialogController;
        StoreErrorDialogController mCannotContactServerDialogController;
        StoreErrorDialogController mPaymentsDisabledOnDeviceDialogController;
        bool mHasFetchedProducts {false};
        float mElapsedTime = 0.0f;
    };
}

#endif
