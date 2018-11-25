#include "SettingsMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "Settings.hpp"

using namespace RowBlast;

SettingsMenuController::SettingsMenuController(Pht::IEngine& engine,
                                               const CommonResources& commonResources,
                                               Settings& settings,
                                               PotentiallyZoomedScreen potentiallyZoomedScreen) :
    mEngine {engine},
    mSettings {settings},
    mView {engine, commonResources, potentiallyZoomedScreen},
    mSlidingMenuAnimation {engine, mView} {}

void SettingsMenuController::SetUp(SlidingMenuAnimation::UpdateFade updateFade,
                                   bool isGestureControlsAllowed) {
    mUpdateFade = updateFade;

    if (isGestureControlsAllowed) {
        mView.EnableControlsButton();
    } else {
        mView.DisableControlsButton();
    }

    mSlidingMenuAnimation.SetUp(updateFade, SlidingMenuAnimation::SlideDirection::Left);
    UpdateViewToReflectSettings(isGestureControlsAllowed);
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
    return InputUtil::HandleInput<Result>(mEngine.GetInput(),
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

SettingsMenuController::Result SettingsMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.IsControlsButtonEnabled()) {
        if (mView.GetControlsButton().IsClicked(touchEvent)) {
            if (mSettings.mControlType == ControlType::Click) {
                mSettings.mControlType = ControlType::Gesture;
            } else {
                mSettings.mControlType = ControlType::Click;
            }
            
            UpdateViewToReflectSettings(true);
        }
    }

    if (mView.GetSoundButton().IsClicked(touchEvent)) {
        auto& audio {mEngine.GetAudio()};
        
        if (audio.IsSoundEnabled()) {
            audio.DisableSound();
        } else {
            audio.EnableSound();
        }
        
        UpdateViewToReflectSettings(true);
    }

    if (mView.GetBackButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoBack;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFade,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}

void SettingsMenuController::UpdateViewToReflectSettings(bool isGestureControlsAllowed) {
    if (mSettings.mControlType == ControlType::Click || !isGestureControlsAllowed) {
        mView.GetControlsClickText().SetIsVisible(true);
        mView.GetControlsSwipeText().SetIsVisible(false);
    } else {
        mView.GetControlsClickText().SetIsVisible(false);
        mView.GetControlsSwipeText().SetIsVisible(true);
    }
    
    auto& audio {mEngine.GetAudio()};

    if (audio.IsSoundEnabled()) {
        mView.GetSoundOnText().SetIsVisible(true);
        mView.GetSoundOffText().SetIsVisible(false);
    } else {
        mView.GetSoundOnText().SetIsVisible(false);
        mView.GetSoundOffText().SetIsVisible(true);
    }
}
