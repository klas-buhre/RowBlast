#include "RestartConfirmationDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

RestartConfirmationDialogController::
RestartConfirmationDialogController(Pht::IEngine& engine,
                                    const CommonResources& commonResources,
                                    const UserServices& userServices) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void RestartConfirmationDialogController::SetUp() {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale);
}

void RestartConfirmationDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

void RestartConfirmationDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

RestartConfirmationDialogController::Result RestartConfirmationDialogController::Update() {
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

RestartConfirmationDialogController::Result RestartConfirmationDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

RestartConfirmationDialogController::Result
RestartConfirmationDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetYesButton().IsClicked(touchEvent)) {
        if (mUserServices.GetLifeService().GetNumLives() == 0) {
            mDeferredResult = Result::RestartLevel;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                                SlidingMenuAnimation::SlideDirection::Left);
        } else {
            return Result::RestartLevel;
        }
    }

    if (mView.GetNoButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::DoNotRestartGame;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Right);
        return Result::None;
    }
    
    return Result::None;
}
