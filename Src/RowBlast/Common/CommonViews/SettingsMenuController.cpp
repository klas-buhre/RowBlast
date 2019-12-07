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
            mView.EnableDragControls();
            break;
        case ControlType::Click:
            mView.EnableClickControls();
            break;
        case ControlType::Swipe:
            mView.EnableSwipeControls();
            break;
        default:
            mView.EnableDragControls();
            break;
    }

    if (!isGestureControlsAllowed) {
        mView.EnableClickControls();
    }

    mView.SetGhostPieceIsEnabled(settingsService.IsGhostPieceEnabled());
    mView.SetRotateAllIsEnabled(settingsService.IsRotateAllPiecesEnabled());
    
    auto& audio = mEngine.GetAudio();
    mView.SetSoundIsOn(audio.IsSoundEnabled());
    mView.SetMusicIsOn(audio.IsMusicEnabled());
}
