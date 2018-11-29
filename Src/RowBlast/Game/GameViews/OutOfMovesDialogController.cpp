#include "OutOfMovesDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

OutOfMovesDialogController::OutOfMovesDialogController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources,
                                                       const UserServices& userServices) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OutOfMovesDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void OutOfMovesDialogController::SetUp(SlidingMenuAnimation::SlideDirection slideDirection,
                                       SlidingMenuAnimation::UpdateFade updateFade) {
    mSlidingMenuAnimation.SetUp(updateFade, slideDirection);
}

OutOfMovesDialogController::Result OutOfMovesDialogController::Update() {
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

OutOfMovesDialogController::Result OutOfMovesDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

OutOfMovesDialogController::Result OutOfMovesDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        return Result::BackToMap;
    }

    if (mView.GetPlayOnButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::PlayOn;
        
        if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::addMovesPriceInCoins)) {
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                                SlidingMenuAnimation::SlideDirection::Right);
        } else {
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                SlidingMenuAnimation::SlideDirection::Left);
        }
    }
    
    return Result::None;
}
