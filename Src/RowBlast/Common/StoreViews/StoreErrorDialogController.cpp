#include "StoreErrorDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

StoreErrorDialogController::StoreErrorDialogController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources,
                                                       const std::string& caption,
                                                       const std::vector<std::string>& textLines,
                                                       PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, caption, textLines, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void StoreErrorDialogController::SetUp(SlidingMenuAnimation::UpdateFade updateFade) {
    mUpdateFade = updateFade;
    mSlidingMenuAnimation.SetUp(updateFade, SlidingMenuAnimation::SlideDirection::Scale);
}

void StoreErrorDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

StoreErrorDialogController::Result StoreErrorDialogController::Update() {
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

StoreErrorDialogController::Result StoreErrorDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

StoreErrorDialogController::Result
StoreErrorDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent) || mView.GetOkButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFade,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }

    return Result::None;
}
