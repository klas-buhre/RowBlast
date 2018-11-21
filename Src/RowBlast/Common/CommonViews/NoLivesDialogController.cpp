#include "NoLivesDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

NoLivesDialogController::NoLivesDialogController(Pht::IEngine& engine,
                                                 const CommonResources& commonResources,
                                                 const UserServices& userServices,
                                                 PotentiallyZoomedScreen potentiallyZoomedScreen) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources, userServices, potentiallyZoomedScreen},
    mSlidingMenuAnimation {engine, mView} {}

void NoLivesDialogController::SetUp(bool shouldSlideOut) {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Up);
    mShouldSlideOut = shouldSlideOut;
}

NoLivesDialogController::Result NoLivesDialogController::Update() {
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            mView.Update();
            if (mUserServices.GetLifeService().GetNumLives() > 0) {
                mDeferredResult = Result::Close;
                mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                    SlidingMenuAnimation::SlideDirection::Down);
            }
            return HandleInput();
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

NoLivesDialogController::Result NoLivesDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

NoLivesDialogController::Result NoLivesDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        if (mShouldSlideOut) {
            mDeferredResult = Result::Close;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                SlidingMenuAnimation::SlideDirection::Down);
            return Result::None;
        }
        
        return Result::Close;
    }

    if (mView.GetRefillLivesButton().IsClicked(touchEvent)) {
        if (mShouldSlideOut) {
            mDeferredResult = Result::RefillLives;
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                SlidingMenuAnimation::SlideDirection::Down);
            return Result::None;
        }
        
        return Result::RefillLives;
    }
    
    return Result::None;
}
