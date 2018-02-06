#include "GameMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "GameLogic.hpp"

using namespace BlocksGame;

GameMenuController::GameMenuController(Pht::IEngine& engine,
                                       const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void GameMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void GameMenuController::Init(SlidingMenuAnimation::UpdateFade updateFade,
                              bool isUndoMovePossible) {
    auto slideInDirection {
        updateFade == SlidingMenuAnimation::UpdateFade::Yes ?
            SlidingMenuAnimation::SlideDirection::Up :
            SlidingMenuAnimation::SlideDirection::Down
    };
    
    mSlidingMenuAnimation.Init(updateFade, slideInDirection);
    
    if (isUndoMovePossible) {
        mView.EnableUndoButton();
    } else {
        mView.DisableUndoButton();
    }
}

GameMenuController::Result GameMenuController::Update() {
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

GameMenuController::Result GameMenuController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

GameMenuController::Result GameMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetResumeButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::ResumeGame;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes);
        return Result::None;
    }

    if (mView.IsUndoButtonEnabled()) {
        if (mView.GetUndoButton().IsClicked(touchEvent)) {
            mDeferredResult = Result::ResumeGame;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes);
            return Result::UndoMove;
        }
    }
    
    if (mView.GetRestartButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::RestartGame;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Up);
        return Result::None;
    }

    if (mView.GetSettingsButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoToSettingsMenu;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Up);
        return Result::None;
    }

    if (mView.GetMapButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::BackToMap;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Up);
        return Result::None;
    }
    
    return Result::None;
}
