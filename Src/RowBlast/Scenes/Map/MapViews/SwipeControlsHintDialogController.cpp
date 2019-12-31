#include "SwipeControlsHintDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

SwipeControlsHintDialogController::SwipeControlsHintDialogController(Pht::IEngine& engine,
                                                                     const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void SwipeControlsHintDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void SwipeControlsHintDialogController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::SlideDirection::Scale);
}

SwipeControlsHintDialogController::Result SwipeControlsHintDialogController::Update() {
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

SwipeControlsHintDialogController::Result SwipeControlsHintDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

SwipeControlsHintDialogController::Result
SwipeControlsHintDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Left);
    }

    if (mView.GetHowToPlayButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Left);
    }

    return Result::None;
}
