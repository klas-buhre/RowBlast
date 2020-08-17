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
                                               SettingsMenuView::SceneId sceneId) :
    mEngine {engine},
    mUserServices {userServices},
    mView {
        engine,
        commonResources,
        sceneId == SettingsMenuView::SceneId::Game ? PotentiallyZoomedScreen::Yes : PotentiallyZoomedScreen::No
    },
    mSlidingMenuAnimation {engine, mView},
    mSceneId {sceneId} {}

void SettingsMenuController::SetUp(bool isControlTypeChangeAllowed) {
    if (isControlTypeChangeAllowed) {
        mView.EnableControlsSection();
    } else {
        mView.DisableControlsSection();
    }

    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Left);
    UpdateViewToReflectSettings();
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
    auto& settingsService = mUserServices.GetSettingsService();

    if (mView.IsControlsSectionEnabled()) {
        if (mView.GetDragAndDropButton().IsClicked(touchEvent)) {
            settingsService.SetControlType(ControlType::Drag);
            UpdateViewToReflectSettings();
        } else if (mView.GetSwipeButton().IsClicked(touchEvent)) {
            settingsService.SetControlType(ControlType::Swipe);
            UpdateViewToReflectSettings();
        } else if (mView.GetSingleTapButton().IsClicked(touchEvent)) {
            settingsService.SetControlType(ControlType::Click);
            UpdateViewToReflectSettings();
        }    
    }

    if (mView.GetGhostPieceButton().IsClicked(touchEvent)) {
        if (settingsService.IsGhostPieceEnabled()) {
            settingsService.SetIsGhostPieceEnabled(false);
        } else {
            settingsService.SetIsGhostPieceEnabled(true);
        }
        
        UpdateViewToReflectSettings();
    }

    if (mView.GetSoundButton().IsClicked(touchEvent)) {
        auto& audio = mEngine.GetAudio();
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
        auto& audio = mEngine.GetAudio();
        if (audio.IsMusicEnabled()) {
            audio.DisableMusic();
            settingsService.SetIsMusicEnabled(false);
        } else {
            audio.EnableMusic();
            settingsService.SetIsMusicEnabled(true);
        }
        
        UpdateViewToReflectSettings();
    }
    
    if (mView.GetThrowButton().IsClicked(touchEvent)) {
        settingsService.SetClearRowsEffect(ClearRowsEffect::Throw);
        UpdateViewToReflectSettings();
    } else if (mView.GetShrinkButton().IsClicked(touchEvent)) {
        settingsService.SetClearRowsEffect(ClearRowsEffect::Shrink);
        UpdateViewToReflectSettings();
    }

    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::GoBack;
        auto updateFade = mSceneId == SettingsMenuView::SceneId::Map ?
                          SlidingMenuAnimation::UpdateFade::Yes :
                          SlidingMenuAnimation::UpdateFade::No;
        mSlidingMenuAnimation.StartSlideOut(updateFade,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}

void SettingsMenuController::UpdateViewToReflectSettings() {
    mView.DeselectAllControlButtons();
    auto& settingsService = mUserServices.GetSettingsService();
    switch (settingsService.GetControlType()) {
        case ControlType::Drag:
            mView.GetDragAndDropButton().SetIsSelected(true);
            break;
        case ControlType::Click:
            mView.GetSingleTapButton().SetIsSelected(true);
            break;
        case ControlType::Swipe:
            mView.GetSwipeButton().SetIsSelected(true);
            break;
        default:
            mView.GetDragAndDropButton().SetIsSelected(true);
            break;
    }

    mView.SetGhostPieceIsEnabled(settingsService.IsGhostPieceEnabled());
    
    auto& audio = mEngine.GetAudio();
    mView.SetSoundIsOn(audio.IsSoundEnabled());
    mView.SetMusicIsOn(audio.IsMusicEnabled());
    
    mView.DeselectAllClearEffectButtons();
    switch (settingsService.GetClearRowsEffect()) {
        case ClearRowsEffect::Shrink:
            mView.GetShrinkButton().SetIsSelected(true);
            break;
        case ClearRowsEffect::Throw:
            mView.GetThrowButton().SetIsSelected(true);
            break;
        default:
            mView.GetThrowButton().SetIsSelected(true);
            break;
    }
}
