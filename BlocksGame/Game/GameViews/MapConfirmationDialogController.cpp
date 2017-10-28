#include "MapConfirmationDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace BlocksGame;

MapConfirmationDialogController::
MapConfirmationDialogController(Pht::IEngine& engine, const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView, 0.6f} {}

void MapConfirmationDialogController::Reset() {
    mSlidingMenuAnimation.Reset(SlidingMenuAnimation::UpdateFade::No);
}

MapConfirmationDialogController::Result MapConfirmationDialogController::Update() {
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

MapConfirmationDialogController::Result MapConfirmationDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

MapConfirmationDialogController::Result
MapConfirmationDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetYesButton().IsClicked(touchEvent)) {
        return Result::GoToMap;
    }

    if (mView.GetNoButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::DoNotGoToMap;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No);
        return Result::None;
    }
    
    return Result::None;
}
