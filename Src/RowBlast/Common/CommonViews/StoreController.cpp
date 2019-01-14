#include "StoreController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.72f};
    constexpr auto fadeTime {0.3f};
    
    int ToExitCriteriaInCoins(StoreController::TriggerProduct triggerProduct) {
        switch (triggerProduct) {
            case StoreController::TriggerProduct::Moves:
                return PurchasingService::addMovesPriceInCoins;
            case StoreController::TriggerProduct::Lives:
                return PurchasingService::refillLivesPriceInCoins;
            case StoreController::TriggerProduct::Coins:
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
    mPurchaseFailedDialogController {
        engine,
        commonResources,
        ToPotentiallyZoomedScreen(sceneId)
    },
    mPurchaseCanceledDialogController {
        engine,
        commonResources,
        ToPotentiallyZoomedScreen(sceneId)
    } {

    mViewManager.AddView(static_cast<int>(ViewController::StoreMenu), mStoreMenuController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseSuccessfulDialog), mPurchaseSuccessfulDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseFailedDialog), mPurchaseFailedDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseCanceledDialog), mPurchaseCanceledDialogController.GetView());
    
    mStoreMenuController.SetFadeEffect(mFadeEffect);
    mPurchaseSuccessfulDialogController.SetFadeEffect(mFadeEffect);
}

void StoreController::Init(Pht::SceneObject& parentObject) {
    mState = State::Idle;
    
    mFadeEffect.Reset();
    parentObject.AddChild(mFadeEffect.GetSceneObject());
    
    mSpinningWheelEffect.Init(parentObject);
    mViewManager.Init(parentObject);
    SetActiveViewController(ViewController::None);
}

void StoreController::StartStore(TriggerProduct triggerProduct,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnStart,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnClose,
                                 SlidingMenuAnimation::UpdateFade updateFadeOnCanAffordTriggerProduct,
                                 PurchaseSuccessfulDialogController::ShouldSlideOut slideOutOnCanAffordTriggerProduct) {
    mTriggerProduct = triggerProduct;
    mUpdateFadeOnClose = updateFadeOnClose;
    mUpdateFadeOnCanAffordTriggerProduct = updateFadeOnCanAffordTriggerProduct;
    mSlideOutOnCanAffordTriggerProduct = slideOutOnCanAffordTriggerProduct;
    GoToStoreMenuState(updateFadeOnStart, SlidingMenuAnimation::SlideDirection::Left);
}

StoreController::Result StoreController::Update() {
    StoreController::Result result {Result::None};
    
    switch (mState) {
        case State::StoreMenu:
            result = UpdateStoreMenu();
            break;
        case State::PurchasePending:
            mSpinningWheelEffect.Update();
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
    auto menuControllerResult {mStoreMenuController.Update()};

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

void StoreController::StartPurchase(ProductId productId) {
    mSpinningWheelEffect.Start();
    
    auto& purchasingService {mUserServices.GetPurchasingService()};
    purchasingService.StartPurchase(productId,
                                    [this] (const GoldCoinProduct& product) {
                                        mSpinningWheelEffect.Stop();
                                        GoToPurchaseSuccessfulDialogState(product);
                                    },
                                    [this] (PurchaseFailureReason purchaseFailureReason) {
                                        mSpinningWheelEffect.Stop();
                                        OnPurchaseFailed(purchaseFailureReason);
                                    });
    GoToPurchasePendingState();
}

void StoreController::OnPurchaseFailed(PurchaseFailureReason purchaseFailureReason) {
    switch (purchaseFailureReason) {
        case PurchaseFailureReason::UserCancelled:
            GoToPurchaseCanceledDialogState();
            break;
        case PurchaseFailureReason::Other:
            GoToPurchaseFailedDialogState();
            break;
    }
}

StoreController::Result StoreController::UpdatePurchaseSuccessfulDialog() {
    StoreController::Result result {Result::None};

    switch (mPurchaseSuccessfulDialogController.Update()) {
        case PurchaseSuccessfulDialogController::Result::None:
            break;
        case PurchaseSuccessfulDialogController::Result::Close:
            if (mUserServices.GetPurchasingService().CanAfford(ToExitCriteriaInCoins(mTriggerProduct))) {
                result = Result::Done;
            } else {
                GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No,
                                   SlidingMenuAnimation::SlideDirection::Right);
            }
            break;
    }
    
    return result;
}

void StoreController::UpdatePurchaseFailedDialog() {
    switch (mPurchaseFailedDialogController.Update()) {
        case PurchaseFailedDialogController::Result::None:
            break;
        case PurchaseFailedDialogController::Result::Close:
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

void StoreController::GoToPurchaseSuccessfulDialogState(const GoldCoinProduct& product) {
    mState = State::PurchaseSuccessfulDialog;
    SetActiveViewController(ViewController::PurchaseSuccessfulDialog);

    if (mUserServices.GetPurchasingService().CanAfford(ToExitCriteriaInCoins(mTriggerProduct))) {
        mPurchaseSuccessfulDialogController.SetUp(product.mNumCoins,
                                                  mSlideOutOnCanAffordTriggerProduct,
                                                  mUpdateFadeOnCanAffordTriggerProduct);
    } else {
        mPurchaseSuccessfulDialogController.SetUp(product.mNumCoins,
                                                  PurchaseSuccessfulDialogController::ShouldSlideOut::Yes,
                                                  SlidingMenuAnimation::UpdateFade::No);
    }
}

void StoreController::GoToPurchaseFailedDialogState() {
    mState = State::PurchaseFailedDialog;
    SetActiveViewController(ViewController::PurchaseFailedDialog);
    mPurchaseFailedDialogController.SetUp();
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

void StoreController::GoToPurchasePendingState() {
    mState = State::PurchasePending;
    SetActiveViewController(ViewController::None);
}