#include "StoreController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "NetworkStatus.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade = 0.72f;
    constexpr auto fadeTime = 0.3f;
    constexpr auto purchaseSuccessfulDelay = 0.15f;
    
    int ToExitCriteriaInCoins(StoreTrigger storeTrigger) {
        switch (storeTrigger) {
            case StoreTrigger::Moves:
                return PurchasingService::addMovesPriceInCoins;
            case StoreTrigger::Lives:
                return PurchasingService::refillLivesPriceInCoins;
            case StoreTrigger::Coins:
                return 0;
        }
    }
    
    PotentiallyZoomedScreen ToPotentiallyZoomedScreen(StoreController::SceneId sceneId) {
        switch (sceneId) {
            case StoreController::SceneId::Map:
                return PotentiallyZoomedScreen::No;
            case StoreController::SceneId::Game:
                return PotentiallyZoomedScreen::Yes;
        }
    }
}

StoreController::StoreController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 UserServices& userServices,
                                 IGuiLightProvider& guiLightProvider,
                                 SceneId sceneId) :
    mEngine {engine},
    mUserServices {userServices},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mSpinningWheelEffect {engine},
    mStoreMenuController {
        engine,
        commonResources,
        userServices,
        guiLightProvider,
        ToPotentiallyZoomedScreen(sceneId)
    },
    mPurchaseSuccessfulDialogController {
        engine,
        commonResources,
        ToPotentiallyZoomedScreen(sceneId)
    },
    mPurchaseCanceledDialogController {
        engine,
        commonResources,
        ToPotentiallyZoomedScreen(sceneId)
    },
    mNoInternetAccessDialogController {
        engine,
        commonResources,
        ToPotentiallyZoomedScreen(sceneId)
    },
    mPurchaseFailedDialogController {
        engine,
        commonResources,
        "PURCHASE FAILED",
        {"No money was removed from", " your account."},
        ToPotentiallyZoomedScreen(sceneId)
    },
    mCannotContactServerDialogController {
        engine,
        commonResources,
        "CAN'T CONTACT SERVER",
        {"No response from server.", "Try again later."},
        ToPotentiallyZoomedScreen(sceneId)
    },
    mPaymentsDisabledOnDeviceDialogController {
        engine,
        commonResources,
        "PAYMENT NOT ALLOWED",
        {"Payments are disabled on", "this device."},
        ToPotentiallyZoomedScreen(sceneId)
    } {

    mViewManager.AddView(static_cast<int>(ViewController::StoreMenu), mStoreMenuController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseSuccessfulDialog), mPurchaseSuccessfulDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseFailedDialog), mPurchaseFailedDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseCanceledDialog), mPurchaseCanceledDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::NoInternetAccessDialog), mNoInternetAccessDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::CannotContactServerDialog), mCannotContactServerDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PaymentsDisabledOnDeviceDialog), mPaymentsDisabledOnDeviceDialogController.GetView());
    
    mStoreMenuController.SetFadeEffect(mFadeEffect);
    mPurchaseSuccessfulDialogController.SetFadeEffect(mFadeEffect);
    mNoInternetAccessDialogController.SetFadeEffect(mFadeEffect);
    mCannotContactServerDialogController.SetFadeEffect(mFadeEffect);
}

void StoreController::Init(Pht::SceneObject& parentObject) {
    mState = State::Idle;
    
    mFadeEffect.Reset();
    parentObject.AddChild(mFadeEffect.GetSceneObject());
    
    mSpinningWheelEffect.Init(parentObject);
    mViewManager.Init(parentObject);
    SetActiveViewController(ViewController::None);
}

void StoreController::StartStore(StoreTrigger storeTrigger,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnStart,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnClose,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnCanAffordTriggerProduct,
                                 PurchaseSuccessfulDialogController::ShouldSlideOut slideOutOnCanAffordTriggerProduct) {
    mStoreTrigger = storeTrigger;
    mUpdateFadeOnClose = updateFadeOnClose;
    mUpdateFadeOnCanAffordTriggerProduct = updateFadeOnCanAffordTriggerProduct;
    mSlideOutOnCanAffordTriggerProduct = slideOutOnCanAffordTriggerProduct;
    
    if (mHasFetchedProducts) {
        GoToStoreMenuState(updateFadeOnStart, SlidingMenuAnimation::SlideDirection::Left);
    } else {
        FetchProducts(updateFadeOnStart);
    }
}

StoreController::Result StoreController::Update() {
    StoreController::Result result {Result::None};
    
    switch (mState) {
        case State::StoreMenu:
            result = UpdateStoreMenu();
            break;
        case State::FetchingProducts:
        case State::PurchasePending:
            mSpinningWheelEffect.Update();
            break;
        case State::NoInternetAccessDialog:
            result = UpdateNoInternetAccessDialog();
            break;
        case State::CannotContactServerDialog:
            result = UpdateCannotContactServerDialog();
            break;
        case State::PurchaseSuccessfulDelay:
            UpdateInPurchaseSuccessfulDelayState();
            break;
        case State::PurchaseSuccessfulDialog:
            result = UpdatePurchaseSuccessfulDialog();
            break;
        case State::PurchaseFailedDialog:
            UpdatePurchaseFailedDialog();
            break;
        case State::PurchaseCanceledDialog:
            UpdatePurchaseCanceledDialog();
            break;
        case State::PaymentsDisabledOnDeviceDialog:
            UpdatePaymentsDisabledOnDeviceDialog();
            break;
        case State::NoInternetAccessAtPurchaseAttemptDialog:
            UpdateNoInternetAccessAtPurchaseAttemptDialog();
            break;
        case State::Idle:
            break;
    }
    
    if (result == Result::Done) {
        if (mState == State::PurchaseSuccessfulDialog &&
            mSlideOutOnCanAffordTriggerProduct == PurchaseSuccessfulDialogController::ShouldSlideOut::No) {
            
            // Don't hide the purchase successful dialog if the dialog should not slide out when the
            // store is done.
        } else {
            GoToIdleState();
        }
    }

    return result;
}

StoreController::Result StoreController::UpdateStoreMenu() {
    StoreController::Result result {Result::None};
    auto menuControllerResult = mStoreMenuController.Update();

    switch (menuControllerResult.GetKind()) {
        case StoreMenuController::Result::None:
            break;
        case StoreMenuController::Result::Close:
            result = Result::Done;
            break;
        case StoreMenuController::Result::PurchaseProduct:
            StartPurchase(menuControllerResult.GetProductId());
            break;
    }
    
    return result;
}

void StoreController::FetchProducts(SlidingMenuAnimation::UpdateFade updateFadeOnStart) {
    if (!Pht::NetworkStatus::IsConnected()) {
        GoToNoInternetAccessDialogState(updateFadeOnStart);
        return;
    }

    if (updateFadeOnStart == SlidingMenuAnimation::UpdateFade::Yes) {
        mFadeEffect.SetMidFade();
    }
    
    mSpinningWheelEffect.Start();
    
    mUserServices.GetPurchasingService().FetchProducts(
        [this] (const std::vector<GoldCoinProduct>& products) {
            mSpinningWheelEffect.Stop();
            mStoreMenuController.GetView().SetUpProducts(products);
            mHasFetchedProducts = true;
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Left);
        },
        [this] () {
            mSpinningWheelEffect.Stop();
            GoToCannotContactServerState();
        });
        
    GoToFetchingProductsState();
}

void StoreController::StartPurchase(ProductId productId) {
    mSpinningWheelEffect.Start();
    
    // Hack that prevents the purchase succesful dialog from appearing at the same time as the iOS
    // purchase successful dialog.
    mEngine.GetInput().SetKeepInputRateAt60Hz(false);
    
    mUserServices.GetPurchasingService().StartPurchase(
        productId,
        mStoreTrigger,
        [this] (const GoldCoinProduct& product) {
            mSpinningWheelEffect.Stop();
            mPurchasedProduct = product;
            GoToPurchaseSuccessfulDelayState();
        },
        [this] (Pht::PurchaseError error) {
            mEngine.GetInput().SetKeepInputRateAt60Hz(true);
            mSpinningWheelEffect.Stop();
            OnPurchaseFailed(error);
        });

    GoToPurchasePendingState();
}

void StoreController::OnPurchaseFailed(Pht::PurchaseError error) {
    switch (error) {
        case Pht::PurchaseError::Cancelled:
            GoToPurchaseCanceledDialogState();
            break;
        case Pht::PurchaseError::NotAllowed:
            GoToPaymentsDisabledOnDeviceDialogState();
            break;
        case Pht::PurchaseError::NoNetworkAccess:
            GoToNoInternetAccessAtPurchaseAttemptDialogState();
            break;
        case Pht::PurchaseError::Other:
            if (!Pht::NetworkStatus::IsConnected()) {
                GoToNoInternetAccessAtPurchaseAttemptDialogState();
            } else {
                GoToPurchaseFailedDialogState();
            }
            break;
    }
}

void StoreController::UpdateInPurchaseSuccessfulDelayState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mElapsedTime >= purchaseSuccessfulDelay) {
        GoToPurchaseSuccessfulDialogState();
    }
}

StoreController::Result StoreController::UpdatePurchaseSuccessfulDialog() {
    StoreController::Result result {Result::None};

    switch (mPurchaseSuccessfulDialogController.Update()) {
        case PurchaseSuccessfulDialogController::Result::None:
            break;
        case PurchaseSuccessfulDialogController::Result::Close:
            if (mUserServices.GetPurchasingService().CanAfford(ToExitCriteriaInCoins(mStoreTrigger))) {
                result = Result::Done;
            } else {
                GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                                   SlidingMenuAnimation::SlideDirection::Right);
            }
            break;
    }
    
    return result;
}

StoreController::Result StoreController::UpdateNoInternetAccessDialog() {
    StoreController::Result result {Result::None};
    
    switch (mNoInternetAccessDialogController.Update()) {
        case NoInternetAccessDialogController::Result::None:
            break;
        case NoInternetAccessDialogController::Result::Close:
            result = Result::Done;
            break;
    }
    
    return result;
}

void StoreController::UpdateNoInternetAccessAtPurchaseAttemptDialog() {
    switch (mNoInternetAccessDialogController.Update()) {
        case NoInternetAccessDialogController::Result::None:
            break;
        case NoInternetAccessDialogController::Result::Close:
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

StoreController::Result StoreController::UpdateCannotContactServerDialog() {
    StoreController::Result result {Result::None};
    
    switch (mCannotContactServerDialogController.Update()) {
        case StoreErrorDialogController::Result::None:
            break;
        case StoreErrorDialogController::Result::Close:
            result = Result::Done;
            break;
    }
    
    return result;
}

void StoreController::UpdatePaymentsDisabledOnDeviceDialog() {
    switch (mPaymentsDisabledOnDeviceDialogController.Update()) {
        case StoreErrorDialogController::Result::None:
            break;
        case StoreErrorDialogController::Result::Close:
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

void StoreController::UpdatePurchaseFailedDialog() {
    switch (mPurchaseFailedDialogController.Update()) {
        case StoreErrorDialogController::Result::None:
            break;
        case StoreErrorDialogController::Result::Close:
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

void StoreController::UpdatePurchaseCanceledDialog() {
    switch (mPurchaseCanceledDialogController.Update()) {
        case PurchaseCanceledDialogController::Result::None:
            break;
        case PurchaseCanceledDialogController::Result::Close:
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Right);
            break;
    }
}

void StoreController::SetActiveViewController(ViewController viewController) {
    if (viewController == ViewController::None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(viewController));
    }
}

void StoreController::GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade,
                                         SlidingMenuAnimation::SlideDirection slideDirection) {
    mState = State::StoreMenu;
    SetActiveViewController(ViewController::StoreMenu);
    mStoreMenuController.SetUp(updateFade, mUpdateFadeOnClose, slideDirection);
}

void StoreController::GoToPurchaseSuccessfulDelayState() {
    mState = State::PurchaseSuccessfulDelay;
    mElapsedTime = 0.0f;
    SetActiveViewController(ViewController::None);
}

void StoreController::GoToPurchaseSuccessfulDialogState() {
    mEngine.GetInput().SetKeepInputRateAt60Hz(true);
    mState = State::PurchaseSuccessfulDialog;
    SetActiveViewController(ViewController::PurchaseSuccessfulDialog);

    if (mUserServices.GetPurchasingService().CanAfford(ToExitCriteriaInCoins(mStoreTrigger))) {
        mPurchaseSuccessfulDialogController.SetUp(mPurchasedProduct.mNumCoins,
                                                  mSlideOutOnCanAffordTriggerProduct,
                                                  mUpdateFadeOnCanAffordTriggerProduct);
    } else {
        mPurchaseSuccessfulDialogController.SetUp(mPurchasedProduct.mNumCoins,
                                                  PurchaseSuccessfulDialogController::ShouldSlideOut::Yes,
                                                  SlidingMenuAnimation::UpdateFade::No);
    }
}

void StoreController::GoToNoInternetAccessDialogState(SlidingMenuAnimation::UpdateFade updateFade) {
    mState = State::NoInternetAccessDialog;
    SetActiveViewController(ViewController::NoInternetAccessDialog);
    mNoInternetAccessDialogController.SetUp(updateFade, updateFade);
}

void StoreController::GoToNoInternetAccessAtPurchaseAttemptDialogState() {
    mState = State::NoInternetAccessAtPurchaseAttemptDialog;
    SetActiveViewController(ViewController::NoInternetAccessDialog);
    mNoInternetAccessDialogController.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::UpdateFade::No);
}

void StoreController::GoToCannotContactServerState() {
    mState = State::CannotContactServerDialog;
    SetActiveViewController(ViewController::CannotContactServerDialog);
    mCannotContactServerDialogController.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                               mUpdateFadeOnClose);
}

void StoreController::GoToPaymentsDisabledOnDeviceDialogState() {
    mState = State::PaymentsDisabledOnDeviceDialog;
    SetActiveViewController(ViewController::PaymentsDisabledOnDeviceDialog);
    mPaymentsDisabledOnDeviceDialogController.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                                    SlidingMenuAnimation::UpdateFade::No);
}

void StoreController::GoToPurchaseFailedDialogState() {
    mState = State::PurchaseFailedDialog;
    SetActiveViewController(ViewController::PurchaseFailedDialog);
    mPurchaseFailedDialogController.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                          SlidingMenuAnimation::UpdateFade::No);
}

void StoreController::GoToPurchaseCanceledDialogState() {
    mState = State::PurchaseCanceledDialog;
    SetActiveViewController(ViewController::PurchaseCanceledDialog);
    mPurchaseCanceledDialogController.SetUp();
}

void StoreController::GoToIdleState() {
    mState = State::Idle;
    SetActiveViewController(ViewController::None);
}

void StoreController::GoToFetchingProductsState() {
    mState = State::FetchingProducts;
    SetActiveViewController(ViewController::None);
}

void StoreController::GoToPurchasePendingState() {
    mState = State::PurchasePending;
    SetActiveViewController(ViewController::None);
}
