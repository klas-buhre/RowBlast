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
    } {

    mViewManager.AddView(static_cast<int>(ViewController::StoreMenu), mStoreMenuController.GetView());
}

void StoreController::Init(Pht::SceneObject& parentObject) {
    mState = State::Idle;
    
    mFadeEffect.Reset();
    mStoreMenuController.SetFadeEffect(mFadeEffect);
    parentObject.AddChild(mFadeEffect.GetSceneObject());
 
    mViewManager.Init(parentObject);
    SetActiveViewController(ViewController::None);
}

void StoreController::StartPurchaseFlow(Trigger trigger) {
    mTrigger = trigger;
    mState = State::StoreMenu;
    SetActiveViewController(ViewController::StoreMenu);
    mStoreMenuController.Init();
}

StoreController::Result StoreController::Update() {
    switch (mState) {
        case State::StoreMenu:
            break;
        case State::Idle:
            break;
    }

    return Result::None;
}

void StoreController::UpdateStoreMenu() {
    switch (mStoreMenuController.Update()) {
        case StoreMenuController::Result::None:
            break;
        case StoreMenuController::Result::Close:
            // GoToOutOfMovesStateOutOfMovesDialog();
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
