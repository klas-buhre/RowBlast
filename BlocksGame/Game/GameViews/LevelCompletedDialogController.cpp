#include "LevelCompletedDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "CommonResources.hpp"

using namespace BlocksGame;

LevelCompletedDialogController::LevelCompletedDialogController(Pht::IEngine& engine,
                                                               const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView, 0.6f} {}

void LevelCompletedDialogController::Reset() {
    mSlidingMenuAnimation.Reset();
    mView.Reset();
}

LevelCompletedDialogController::Result LevelCompletedDialogController::Update() {
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
        case SlidingMenuAnimation::State::Done:
            break;
        case SlidingMenuAnimation::State::ShowingMenu: {
            mView.Update();
            return HandleInput();
        }
    }
    
    return Result::None;
}

LevelCompletedDialogController::Result LevelCompletedDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

LevelCompletedDialogController::Result
LevelCompletedDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        return Result::BackToMap;
    }

    if (mView.GetNextButton().IsClicked(touchEvent)) {
        return Result::NextLevel;
    }
    
    return Result::None;
}
