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

void NoLivesDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

void NoLivesDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void NoLivesDialogController::SetUp(SlidingMenuAnimation::UpdateFade updateFadeOnStart,
                                    ShouldSlideOut shouldSlideOutOnClose,
                                    ShouldSlideOut shouldSlideOutOnRefillLives) {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(updateFadeOnStart, SlidingMenuAnimation::SlideDirection::Left);

    mShouldSlideOutOnClose = shouldSlideOutOnClose;
    mShouldSlideOutOnRefillLives = shouldSlideOutOnRefillLives;
    mHasResult = false;
}

NoLivesDialogController::Result NoLivesDialogController::Update() {
    mView.Update();
    
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu: {
            if (!mHasResult && mUserServices.GetLifeService().GetNumLives() > 0) {
                if (mShouldSlideOutOnClose == ShouldSlideOut::Yes) {
                    SetDeferredResult(Result::Close);
                    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                                        SlidingMenuAnimation::SlideDirection::Right);
                    return Result::None;
                }
                return ReturnResult(Result::Close);
            }
            return HandleInput();
        }
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

NoLivesDialogController::Result NoLivesDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

NoLivesDialogController::Result NoLivesDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        if (mShouldSlideOutOnClose == ShouldSlideOut::Yes) {
            SetDeferredResult(Result::Close);
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                                SlidingMenuAnimation::SlideDirection::Right);
            return Result::None;
        }
        
        return ReturnResult(Result::Close);
    }

    if (mView.GetRefillLivesButton().IsClicked(touchEvent)) {
        if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
            if (mShouldSlideOutOnRefillLives == ShouldSlideOut::Yes) {
                SetDeferredResult(Result::RefillLives);
                mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                                    SlidingMenuAnimation::SlideDirection::Right);
                return Result::None;
            }
        
            return ReturnResult(Result::RefillLives);
        } else {
            SetDeferredResult(Result::RefillLives);
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                SlidingMenuAnimation::SlideDirection::Left);
            return Result::None;
        }
    }
    
    return Result::None;
}

NoLivesDialogController::Result NoLivesDialogController::ReturnResult(Result result) {
    mHasResult = true;
    return result;
}

void NoLivesDialogController::SetDeferredResult(Result result) {
    mHasResult = true;
    mDeferredResult = result;
}
