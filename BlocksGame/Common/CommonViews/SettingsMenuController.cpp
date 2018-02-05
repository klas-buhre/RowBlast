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
    mSlidingMenuAnimation {engine, mView} {}

void SettingsMenuController::Init(SlidingMenuAnimation::UpdateFade updateFade) {
    mUpdateFade = updateFade;
    mSlidingMenuAnimation.Init(updateFade);
}

void SettingsMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
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
        if (mSettings.mControlType == ControlType::Click) {
            mSettings.mControlType = ControlType::Gesture;
            mView.GetControlsClickText().SetIsVisible(false);
            mView.GetControlsSwipeText().SetIsVisible(true);
        } else {
            mSettings.mControlType = ControlType::Click;
            mView.GetControlsClickText().SetIsVisible(true);
            mView.GetControlsSwipeText().SetIsVisible(false);
        }
    }

    if (mView.GetSoundButton().IsClicked(touchEvent)) {
        auto& audio {mEngine.GetAudio()};
        
        if (audio.IsSoundEnabled()) {
            audio.DisableSound();
            mView.GetSoundOnText().SetIsVisible(false);
            mView.GetSoundOffText().SetIsVisible(true);
        } else {
            audio.EnableSound();
            mView.GetSoundOnText().SetIsVisible(true);
            mView.GetSoundOffText().SetIsVisible(false);
        }
    }

    if (mView.GetBackButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoBack;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFade);
    }
    
    return Result::None;
}
