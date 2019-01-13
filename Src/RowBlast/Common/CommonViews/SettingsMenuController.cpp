#include "SettingsMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

SettingsMenuController::SettingsMenuController(Pht::IEngine& engine,
                                               const CommonResources& commonResources,
                                               UserServices& userServices,
                                               PotentiallyZoomedScreen potentiallyZoomedScreen) :
    mEngine {engine},
    mUserServices {userServices},
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
    auto& settingsService {mUserServices.GetSettingsService()};

    if (mView.IsControlsButtonEnabled()) {
        if (mView.GetControlsButton().IsClicked(touchEvent)) {
            if (settingsService.GetControlType() == ControlType::Click) {
                settingsService.SetControlType(ControlType::Gesture);
            } else {
                settingsService.SetControlType(ControlType::Click);
            }
            
            UpdateViewToReflectSettings(true);
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
        
        UpdateViewToReflectSettings(true);
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
    auto& settingsService {mUserServices.GetSettingsService()};
    
    if (settingsService.GetControlType() == ControlType::Click || !isGestureControlsAllowed) {
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
    
    if (audio.IsMusicEnabled()) {
        mView.GetMusicOnText().SetIsVisible(true);
        mView.GetMusicOffText().SetIsVisible(false);
    } else {
        mView.GetMusicOnText().SetIsVisible(false);
        mView.GetMusicOffText().SetIsVisible(true);
    }
}
