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
    mView {engine, commonResources, sceneId},
    mSlidingMenuAnimation {engine, mView},
    mSceneId {sceneId} {}

void SettingsMenuController::SetUp(bool isGestureControlsAllowed) {
    if (isGestureControlsAllowed) {
        mView.EnableControlsButton();
    } else {
        mView.DisableControlsButton();
    }

    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Left);
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
    auto& settingsService = mUserServices.GetSettingsService();

    if (mView.IsControlsButtonEnabled()) {
        if (mView.GetControlsButton().IsClicked(touchEvent)) {
            switch (settingsService.GetControlType()) {
                case ControlType::Drag:
                    settingsService.SetControlType(ControlType::Click);
                    break;
                case ControlType::Click:
                    settingsService.SetControlType(ControlType::Swipe);
                    break;
                case ControlType::Swipe:
                    settingsService.SetControlType(ControlType::Drag);
                    break;
                default:
                    settingsService.SetControlType(ControlType::Drag);
                    break;
            }
            
            UpdateViewToReflectSettings(true);
        }
    }

    if (mView.GetGhostPieceButton().IsClicked(touchEvent)) {
        if (settingsService.IsGhostPieceEnabled()) {
            settingsService.SetIsGhostPieceEnabled(false);
        } else {
            settingsService.SetIsGhostPieceEnabled(true);
        }
        
        UpdateViewToReflectSettings(true);
    }

    if (mView.GetRotateAllButton().IsClicked(touchEvent)) {
        if (settingsService.IsRotateAllPiecesEnabled()) {
            settingsService.SetIsRotateAllPiecesEnabled(false);
        } else {
            settingsService.SetIsRotateAllPiecesEnabled(true);
        }
        
        UpdateViewToReflectSettings(true);
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
        
        UpdateViewToReflectSettings(true);
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
        
        UpdateViewToReflectSettings(true);
    }

    if (mView.GetBackButton().IsClicked(touchEvent) ||
        mView.GetCloseButton().IsClicked(touchEvent)) {

        mDeferredResult = Result::GoBack;
        auto updateFade = mSceneId == SettingsMenuView::SceneId::Map ?
                          SlidingMenuAnimation::UpdateFade::Yes :
                          SlidingMenuAnimation::UpdateFade::No;
        mSlidingMenuAnimation.StartSlideOut(updateFade,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}

void SettingsMenuController::UpdateViewToReflectSettings(bool isGestureControlsAllowed) {
    auto& settingsService = mUserServices.GetSettingsService();
    switch (settingsService.GetControlType()) {
        case ControlType::Drag:
            mView.SetClickControlsIsVisible(false);
            mView.SetGestureControlsIsVisible(true);
            break;
        case ControlType::Click:
            mView.SetClickControlsIsVisible(true);
            mView.SetGestureControlsIsVisible(false);
            break;
        case ControlType::Swipe:
            mView.SetClickControlsIsVisible(false);
            mView.SetGestureControlsIsVisible(true);
            break;
        default:
            mView.SetClickControlsIsVisible(false);
            mView.SetGestureControlsIsVisible(true);
            break;
    }

    if (!isGestureControlsAllowed) {
        mView.SetClickControlsIsVisible(true);
        mView.SetGestureControlsIsVisible(false);
    }

    if (settingsService.IsGhostPieceEnabled()) {
        mView.SetGhostPieceOnIsVisible(true);
        mView.SetGhostPieceOffIsVisible(false);
    } else {
        mView.SetGhostPieceOnIsVisible(false);
        mView.SetGhostPieceOffIsVisible(true);
    }

    if (settingsService.IsRotateAllPiecesEnabled()) {
        mView.SetRotateAllOnIsVisible(true);
        mView.SetRotateAllOffIsVisible(false);
    } else {
        mView.SetRotateAllOnIsVisible(false);
        mView.SetRotateAllOffIsVisible(true);
    }

    auto& audio = mEngine.GetAudio();
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
