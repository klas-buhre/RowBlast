#include "CascadingDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

CascadingDialogController::CascadingDialogController(Pht::IEngine& engine,
                                                     const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void CascadingDialogController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::Yes,
                               SlidingMenuAnimation::SlideDirection::Scale,
                               SlidingMenuAnimation::UpdatePosition::No);
}

void CascadingDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

CascadingDialogController::Result CascadingDialogController::Update() {
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

CascadingDialogController::Result CascadingDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

CascadingDialogController::Result CascadingDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Play;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Scale,
                                            SlidingMenuAnimation::UpdatePosition::No);
    }
    
    return Result::None;
}
