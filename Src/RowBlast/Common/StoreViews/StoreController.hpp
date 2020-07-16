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
        void StartStore(TriggerProduct triggerProduct,
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
            None
        };

        Result UpdateStoreMenu();
        void FetchProducts(SlidingMenuAnimation::UpdateFade updateFadeOnStart);
        void StartPurchase(ProductId productId);
        void OnPurchaseFailed(PurchaseFailureReason purchaseFailureReason);
        Result UpdatePurchaseSuccessfulDialog();
        void UpdatePurchaseFailedDialog();
        void UpdatePurchaseCanceledDialog();
        StoreController::Result UpdateNoInternetAccessDialog();
        void SetActiveViewController(ViewController viewController);
        void GoToNoInternetAccessDialogState(SlidingMenuAnimation::UpdateFade updateFade);
        void GoToCannotContactStoreDialog();
        void GoToFetchingProductsState();
        void GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade,
                                SlidingMenuAnimation::SlideDirection slideDirection);
        void GoToPurchaseSuccessfulDialogState(const GoldCoinProduct& product);
        void GoToPurchaseFailedDialogState();
        void GoToPurchaseCanceledDialogState();
        void GoToIdleState();
        void GoToPurchasePendingState();
        
        enum class State {
            FetchingProducts,
            NoInternetAccessDialog,
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
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnClose;
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnCanAffordTriggerProduct;
        PurchaseSuccessfulDialogController::ShouldSlideOut mSlideOutOnCanAffordTriggerProduct;
        SpinningWheelEffect mSpinningWheelEffect;
        Pht::GuiViewManager mViewManager;
        StoreMenuController mStoreMenuController;
        PurchaseSuccessfulDialogController mPurchaseSuccessfulDialogController;
        PurchaseCanceledDialogController mPurchaseCanceledDialogController;
        StoreErrorDialogController mPurchaseFailedDialogController;
        StoreErrorDialogController mNoInternetAccessDialogController;
        bool mHasFetchedProducts {false};
    };
}

#endif
