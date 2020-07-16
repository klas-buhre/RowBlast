#include "NoInternetAccessDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

NoInternetAccessDialogController::NoInternetAccessDialogController(Pht::IEngine& engine,
                                                                   const CommonResources& commonResources,
                                                                   PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void NoInternetAccessDialogController::SetUp(SlidingMenuAnimation::UpdateFade updateFadeOnShow,
                                             SlidingMenuAnimation::UpdateFade updateFadeOnClose) {
    mUpdateFadeOnClose = updateFadeOnClose;
    mSlidingMenuAnimation.SetUp(updateFadeOnShow, SlidingMenuAnimation::SlideDirection::Scale);
}

void NoInternetAccessDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

NoInternetAccessDialogController::Result NoInternetAccessDialogController::Update() {
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

NoInternetAccessDialogController::Result NoInternetAccessDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

NoInternetAccessDialogController::Result
NoInternetAccessDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFadeOnClose,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
