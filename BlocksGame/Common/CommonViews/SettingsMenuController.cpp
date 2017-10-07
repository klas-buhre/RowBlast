#include "SettingsMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "Settings.hpp"

using namespace BlocksGame;

SettingsMenuController::SettingsMenuController(Pht::IEngine& engine,
                                               const CommonResources& commonResources,
                                               Settings& settings) :
    mEngine {engine},
    mSettings {settings},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView, 0.6f} {}

void SettingsMenuController::Reset() {
    mSlidingMenuAnimation.Reset();
}

SettingsMenuController::Result SettingsMenuController::Update() {
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

SettingsMenuController::Result SettingsMenuController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mEngine.GetInput(), [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

SettingsMenuController::Result SettingsMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetControlsButton().IsClicked(touchEvent)) {
        if (mSettings.mControlType == Controls::Click) {
            mSettings.mControlType = Controls::Gesture;
            mView.GetControlsClickText().mIsVisible = false;
            mView.GetControlsSwipeText().mIsVisible = true;
        } else {
            mSettings.mControlType = Controls::Click;
            mView.GetControlsClickText().mIsVisible = true;
            mView.GetControlsSwipeText().mIsVisible = false;
        }
    }

    if (mView.GetSoundButton().IsClicked(touchEvent)) {
        auto& audio {mEngine.GetAudio()};
        
        if (audio.IsSoundEnabled()) {
            audio.DisableSound();
            mView.GetSoundOnText().mIsVisible = false;
            mView.GetSoundOffText().mIsVisible = true;
        } else {
            audio.EnableSound();
            mView.GetSoundOnText().mIsVisible = true;
            mView.GetSoundOffText().mIsVisible = false;
        }
    }

    if (mView.GetBackButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoBack;
        mSlidingMenuAnimation.StartSlideOut();
    }
    
    return Result::None;
}
