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
    mPurchaseUnsuccessfulDialogController {
        engine,
        commonResources,
        sceneId == SceneId::Map ? PotentiallyZoomedScreen::No : PotentiallyZoomedScreen::Yes
    } {

    mViewManager.AddView(static_cast<int>(ViewController::StoreMenu), mStoreMenuController.GetView());
    mViewManager.AddView(static_cast<int>(ViewController::PurchaseUnsuccessfulDialog), mPurchaseUnsuccessfulDialogController.GetView());
}

void StoreController::Init(Pht::SceneObject& parentObject) {
    mState = State::Idle;
    
    mFadeEffect.Reset();
    mStoreMenuController.SetFadeEffect(mFadeEffect);
    parentObject.AddChild(mFadeEffect.GetSceneObject());
 
    mViewManager.Init(parentObject);
    SetActiveViewController(ViewController::None);
}

void StoreController::StartPurchaseFlow(TriggerProduct triggerProduct) {
    mTriggerProduct = triggerProduct;
    GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::Yes);
}

StoreController::Result StoreController::Update() {
    StoreController::Result result {Result::None};
    
    switch (mState) {
        case State::StoreMenu:
            result = UpdateStoreMenu();
            break;
        case State::PurchaseUnsuccessfulDialog:
            UpdatePurchaseUnsuccessfulDialog();
            break;
        case State::Idle:
            break;
    }
    
    if (result == Result::Done) {
        SetActiveViewController(ViewController::None);
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
        case StoreMenuController::Result::PurchaseCoins:
            GoToPurchaseUnsuccessfulDialogState();
            break;
    }
    
    return result;
}

void StoreController::UpdatePurchaseUnsuccessfulDialog() {
    switch (mPurchaseUnsuccessfulDialogController.Update()) {
        case PurchaseUnsuccessfulDialogController::Result::None:
            break;
        case PurchaseUnsuccessfulDialogController::Result::Close:
            GoToStoreMenuState(SlidingMenuAnimation::UpdateFade::No);
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

void StoreController::GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade) {
    mState = State::StoreMenu;
    SetActiveViewController(ViewController::StoreMenu);
    mStoreMenuController.SetUp(updateFade);
}

void StoreController::GoToPurchaseUnsuccessfulDialogState() {
    mState = State::PurchaseUnsuccessfulDialog;
    SetActiveViewController(ViewController::PurchaseUnsuccessfulDialog);
    mPurchaseUnsuccessfulDialogController.SetUp();
}
