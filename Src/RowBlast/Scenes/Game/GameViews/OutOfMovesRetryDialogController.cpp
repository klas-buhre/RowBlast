#include "OutOfMovesRetryDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

OutOfMovesRetryDialogController::OutOfMovesRetryDialogController(Pht::IEngine& engine,
                                                                 const CommonResources& commonResources,
                                                                 const UserServices& userServices) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OutOfMovesRetryDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void OutOfMovesRetryDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

void OutOfMovesRetryDialogController::SetUp(GameScene& scene,
                                            SlidingMenuAnimation::SlideDirection slideDirection,
                                            SlidingMenuAnimation::UpdateFade updateFade) {
    mView.SetUp(scene);
    mSlidingMenuAnimation.SetUp(updateFade, slideDirection);
}

OutOfMovesRetryDialogController::Result OutOfMovesRetryDialogController::Update() {
    mView.Update();

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

OutOfMovesRetryDialogController::Result OutOfMovesRetryDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

OutOfMovesRetryDialogController::Result
OutOfMovesRetryDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        return Result::BackToMap;
    }

    if (mView.GetRetryButton().IsClicked(touchEvent)) {
        return Result::Retry;
    }
    
    return Result::None;
}
