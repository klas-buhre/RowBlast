#include "OutOfMovesDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

OutOfMovesDialogController::OutOfMovesDialogController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OutOfMovesDialogController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Up);
}

OutOfMovesDialogController::Result OutOfMovesDialogController::Update() {
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

OutOfMovesDialogController::Result OutOfMovesDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

OutOfMovesDialogController::Result OutOfMovesDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        return Result::BackToMap;
    }

    if (mView.GetPlayOnButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::PlayOn;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Down);
    }
    
    return Result::None;
}
