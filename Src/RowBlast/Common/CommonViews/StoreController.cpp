#include "StoreController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
}

StoreController::StoreController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 SceneId sceneId) :
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mStoreMenuController {
        engine,
        commonResources,
        sceneId == SceneId::Map ? PotentiallyZoomedScreen::No : PotentiallyZoomedScreen::Yes
    },
    mPurchaseSuccessfulDialogController {
        engine,
        commonResources,
        sceneId == SceneId::Map ? PotentiallyZoomedScreen::No : PotentiallyZoomedScreen::Yes
    },
    mPurchaseUnsuccessfulDialogController {
        engine,
        commonResources,
        sceneId == SceneId::Map ? PotentiallyZoomedScreen::No : PotentiallyZoomedScreen::Yes
    } {

    mViewManager.AddView(static_cast<int>(ViewController::StoreMenu), mStoreMenuController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseSuccessfulDialog), mPurchaseSuccessfulDialogController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseUnsuccessfulDialog), mPurchaseUnsuccessfulDialogController.GetView());
}

void StoreController::Init(Pht::SceneObject& parentObject) {
    mState = State::Idle;
    
    mFadeEffect.Reset();
    mStoreMenuController.SetFadeEffect(mFadeEffect);
    mPurchaseSuccessfulDialogController.SetFadeEffect(mFadeEffect);
    parentObject.AddChild(mFadeEffect.GetSceneObject());
 
    mViewManager.Init(parentObject);
    SetActiveViewController(ViewController::None);
}

void StoreController::StartPurchaseFlow(TriggerProduct triggerProduct) {
    mTriggerProduct = triggerProduct;
    GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::Yes,
                       SlidingMenuAnimation::SlideDirection::Left);
}

StoreController::Result StoreController::Update() {
    StoreController::Result result {Result::None};
    
    switch (mState) {
        case State::StoreMenu:
            result = UpdateStoreMenu();
            break;
        case State::PurchaseSuccessfulDialog:
            result = UpdatePurchaseSuccessfulDialog();
            break;
        case State::PurchaseUnsuccessfulDialog:
            UpdatePurchaseUnsuccessfulDialog();
            break;
        case State::Idle:
            break;
    }
    
    if (result == Result::Done) {
        GoToIdleState();
    }

    return result;
}

StoreController::Result StoreController::UpdateStoreMenu() {
    StoreController::Result result {Result::None};

    switch (mStoreMenuController.Update()) {
        case StoreMenuController::Result::None:
            break;
        case StoreMenuController::Result::Close:
            result = Result::Done;
            break;
        case StoreMenuController::Result::Purchase10Coins:
            GoToPurchaseUnsuccessfulDialogState();
            break;
        case StoreMenuController::Result::Purchase50Coins:
        case StoreMenuController::Result::Purchase100Coins:
        case StoreMenuController::Result::Purchase250Coins:
        case StoreMenuController::Result::Purchase500Coins:
            GoToPurchaseSuccessfulDialogState();
            break;
    }
    
    return result;
}

StoreController::Result StoreController::UpdatePurchaseSuccessfulDialog() {
    StoreController::Result result {Result::None};

    switch (mPurchaseSuccessfulDialogController.Update()) {
        case PurchaseSuccessfulDialogController::Result::None:
            break;
        case PurchaseSuccessfulDialogController::Result::Close:
            result = Result::Done;
            break;
    }
    
    return result;
}

void StoreController::UpdatePurchaseUnsuccessfulDialog() {
    switch (mPurchaseUnsuccessfulDialogController.Update()) {
        case PurchaseUnsuccessfulDialogController::Result::None:
            break;
        case PurchaseUnsuccessfulDialogController::Result::Close:
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
    mStoreMenuController.SetUp(updateFade, slideDirection);
}

void StoreController::GoToPurchaseSuccessfulDialogState() {
    mState = State::PurchaseSuccessfulDialog;
    SetActiveViewController(ViewController::PurchaseSuccessfulDialog);
    mPurchaseSuccessfulDialogController.SetUp(50);
}

void StoreController::GoToPurchaseUnsuccessfulDialogState() {
    mState = State::PurchaseUnsuccessfulDialog;
    SetActiveViewController(ViewController::PurchaseUnsuccessfulDialog);
    mPurchaseUnsuccessfulDialogController.SetUp();
}

void StoreController::GoToIdleState() {
    mState = State::Idle;
    SetActiveViewController(ViewController::None);
}
