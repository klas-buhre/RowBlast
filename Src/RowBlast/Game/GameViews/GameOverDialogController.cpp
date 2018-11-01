#include "GameOverDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

GameOverDialogController::GameOverDialogController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources,
                                                   const UserServices& userServices) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void GameOverDialogController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Up);
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
        if (mUserServices.GetLifeService().GetNumLives() == 0) {
            mDeferredResult = Result::Retry;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No);
            return Result::None;
        }
        
        return Result::Retry;
    }
    
    return Result::None;
}
