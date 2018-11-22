#include "PurchaseUnsuccessfulDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

PurchaseUnsuccessfulDialogController::PurchaseUnsuccessfulDialogController(Pht::IEngine& engine,
                                                                           const CommonResources& commonResources,
                                                                           PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void PurchaseUnsuccessfulDialogController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale);
}

PurchaseUnsuccessfulDialogController::Result PurchaseUnsuccessfulDialogController::Update() {
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            return HandleInput();
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

PurchaseUnsuccessfulDialogController::Result PurchaseUnsuccessfulDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

PurchaseUnsuccessfulDialogController::Result
PurchaseUnsuccessfulDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
