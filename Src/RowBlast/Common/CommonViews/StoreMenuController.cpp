#include "StoreMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

StoreMenuController::StoreMenuController(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         PotentiallyZoomedScreen potentiallyZoomedScreen) :
    mEngine {engine},
    mView {engine, commonResources, potentiallyZoomedScreen},
    mSlidingMenuAnimation {engine, mView} {}

void StoreMenuController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::Yes,
                               SlidingMenuAnimation::SlideDirection::Left);
}

void StoreMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

StoreMenuController::Result StoreMenuController::Update() {
    mView.Update();
    
    auto previousSlidingMenuAnimationState {mSlidingMenuAnimation.GetState()};

    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            if (previousSlidingMenuAnimationState != SlidingMenuAnimation::State::ShowingMenu) {
                mView.StartEffects();
            }
            return HandleInput();
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

StoreMenuController::Result StoreMenuController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mEngine.GetInput(), [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

StoreMenuController::Result StoreMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
        return Result::None;
    }
    
    for (auto& product: mView.GetProducts()) {
        if (product.mButton->IsClicked(touchEvent)) {
        
        }
    }
    
    return Result::None;
}
