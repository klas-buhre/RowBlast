#include "GameOverDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserData.hpp"

using namespace BlocksGame;

GameOverDialogController::GameOverDialogController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources,
                                                   const UserData& userData) :
    mInput {engine.GetInput()},
    mUserData {userData},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView, 0.6f} {}

void GameOverDialogController::Reset() {
    mSlidingMenuAnimation.Reset(SlidingMenuAnimation::UpdateFade::Yes);
}

GameOverDialogController::Result GameOverDialogController::Update() {
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

GameOverDialogController::Result GameOverDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

GameOverDialogController::Result
GameOverDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        return Result::BackToMap;
    }

    if (mView.GetRetryButton().IsClicked(touchEvent)) {
        if (mUserData.GetLifeManager().GetNumLives() == 0) {
            mDeferredResult = Result::Retry;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes);
            return Result::None;
        }
        
        return Result::Retry;
    }
    
    return Result::None;
}
