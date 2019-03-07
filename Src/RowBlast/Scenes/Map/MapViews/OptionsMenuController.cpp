#include "OptionsMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

OptionsMenuController::OptionsMenuController(Pht::IEngine& engine,
                                             const CommonResources& commonResources,
                                             UserServices& userServices) :
    mEngine {engine},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OptionsMenuController::SetUp(bool isGestureControlsAllowed) {
    if (isGestureControlsAllowed) {
        mView.EnableControlsButton();
    } else {
        mView.DisableControlsButton();
    }

    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::SlideDirection::Left);
    UpdateViewToReflectSettings();
}

void OptionsMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

OptionsMenuController::Result OptionsMenuController::Update() {
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

OptionsMenuController::Result OptionsMenuController::HandleInput() {
    return InputUtil::HandleInput<Result>(mEngine.GetInput(),
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

OptionsMenuController::Result OptionsMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    auto& settingsService {mUserServices.GetSettingsService()};

    if (mView.IsControlsButtonEnabled()) {
        if (mView.GetControlsButton().IsClicked(touchEvent)) {
            if (settingsService.GetControlType() == ControlType::Click) {
                settingsService.SetControlType(ControlType::Gesture);
            } else {
                settingsService.SetControlType(ControlType::Click);
            }
            
            UpdateViewToReflectSettings();
        }
    }

    if (mView.GetSoundButton().IsClicked(touchEvent)) {
        auto& audio {mEngine.GetAudio()};
        
        if (audio.IsSoundEnabled()) {
            audio.DisableSound();
            settingsService.SetIsSoundEnabled(false);
        } else {
            audio.EnableSound();
            settingsService.SetIsSoundEnabled(true);
        }
        
        UpdateViewToReflectSettings();
    }

    if (mView.GetMusicButton().IsClicked(touchEvent)) {
        auto& audio {mEngine.GetAudio()};
        
        if (audio.IsMusicEnabled()) {
            audio.DisableMusic();
            settingsService.SetIsMusicEnabled(false);
        } else {
            audio.EnableMusic();
            settingsService.SetIsMusicEnabled(true);
        }
        
        UpdateViewToReflectSettings();
    }
    
    if (mView.GetAboutButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoToAboutMenu;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Left);
    }

    if (mView.GetBackButton().IsClicked(touchEvent) ||
        mView.GetCloseButton().IsClicked(touchEvent)) {

        mDeferredResult = Result::GoBack;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}

void OptionsMenuController::UpdateViewToReflectSettings() {
    auto& settingsService {mUserServices.GetSettingsService()};
    
    if (settingsService.GetControlType() == ControlType::Click) {
        mView.SetControlsClickIsVisible(true);
        mView.SetControlsSwipeIsVisible(false);
    } else {
        mView.SetControlsClickIsVisible(false);
        mView.SetControlsSwipeIsVisible(true);
    }
    
    auto& audio {mEngine.GetAudio()};

    if (audio.IsSoundEnabled()) {
        mView.SetSoundOnIsVisible(true);
        mView.SetSoundOffIsVisible(false);
    } else {
        mView.SetSoundOnIsVisible(false);
        mView.SetSoundOffIsVisible(true);
    }
    
    if (audio.IsMusicEnabled()) {
        mView.SetMusicOnIsVisible(true);
        mView.SetMusicOffIsVisible(false);
    } else {
        mView.SetMusicOnIsVisible(false);
        mView.SetMusicOffIsVisible(true);
    }
}
