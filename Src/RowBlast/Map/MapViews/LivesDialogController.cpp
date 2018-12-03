#include "LivesDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

LivesDialogController::LivesDialogController(Pht::IEngine& engine,
                                             const CommonResources& commonResources,
                                             const UserServices& userServices,
                                             IGuiLightProvider& guiLightProvider) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, userServices, guiLightProvider},
    mSlidingMenuAnimation {engine, mView} {}

void LivesDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void LivesDialogController::SetUp() {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::SlideDirection::Left);
}

LivesDialogController::Result LivesDialogController::Update() {
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

LivesDialogController::Result LivesDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

LivesDialogController::Result LivesDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {

        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
