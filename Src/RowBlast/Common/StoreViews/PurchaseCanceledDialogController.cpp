#include "PurchaseCanceledDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

PurchaseCanceledDialogController::PurchaseCanceledDialogController(Pht::IEngine& engine,
                                                                   const CommonResources& commonResources,
                                                                   PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void PurchaseCanceledDialogController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale);
}

PurchaseCanceledDialogController::Result PurchaseCanceledDialogController::Update() {
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

PurchaseCanceledDialogController::Result PurchaseCanceledDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

PurchaseCanceledDialogController::Result
PurchaseCanceledDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
