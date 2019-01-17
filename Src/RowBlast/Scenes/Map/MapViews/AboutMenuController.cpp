#include "AboutMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

AboutMenuController::AboutMenuController(Pht::IEngine& engine,
                                         const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void AboutMenuController::SetUp(SlidingMenuAnimation::UpdateFade updateFade) {
    mSlidingMenuAnimation.SetUp(updateFade, SlidingMenuAnimation::SlideDirection::Left);
}

void AboutMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

AboutMenuController::Result AboutMenuController::Update() {
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

AboutMenuController::Result AboutMenuController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

AboutMenuController::Result AboutMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetTermsOfServiceButton().IsClicked(touchEvent)) {
        return Result::ViewTermsOfService;
    }

    if (mView.GetPrivacyPolicyButton().IsClicked(touchEvent)) {
        return Result::ViewPrivacyPolicy;
    }

    if (mView.GetCreditsButton().IsClicked(touchEvent)) {
        return Result::ViewCredits;
    }

    if (mView.GetBackButton().IsClicked(touchEvent) ||
        mView.GetCloseButton().IsClicked(touchEvent)) {

        mDeferredResult = Result::GoBack;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}
