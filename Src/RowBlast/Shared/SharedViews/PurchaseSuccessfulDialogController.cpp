#include "PurchaseSuccessfulDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

PurchaseSuccessfulDialogController::PurchaseSuccessfulDialogController(Pht::IEngine& engine,
                                                                       const CommonResources& commonResources,
                                                                       PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void PurchaseSuccessfulDialogController::SetUp(int numCoins,
                                               SlidingMenuAnimation::UpdateFade updateFadeOnClose) {
    mUpdateFadeOnClose = updateFadeOnClose;
    mView.SetUp(numCoins);
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale);
}

void PurchaseSuccessfulDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

PurchaseSuccessfulDialogController::Result PurchaseSuccessfulDialogController::Update() {
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

PurchaseSuccessfulDialogController::Result PurchaseSuccessfulDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

PurchaseSuccessfulDialogController::Result
PurchaseSuccessfulDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFadeOnClose,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
