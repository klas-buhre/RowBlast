#include "OutOfMovesContinueDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

OutOfMovesContinueDialogController::OutOfMovesContinueDialogController(Pht::IEngine& engine,
                                                                       const CommonResources& commonResources,
                                                                       const UserServices& userServices) :
    mInput {engine.GetInput()},
    mUserServices {userServices},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OutOfMovesContinueDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void OutOfMovesContinueDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

void OutOfMovesContinueDialogController::SetUp(GameScene& scene,
                                               SlidingMenuAnimation::SlideDirection slideDirection,
                                               SlidingMenuAnimation::UpdateFade updateFade) {
    mView.SetUp(scene);
    mSlidingMenuAnimation.SetUp(updateFade, slideDirection);
}

OutOfMovesContinueDialogController::Result OutOfMovesContinueDialogController::Update() {
    mView.Update();

    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu: {
            auto result = HandleInput();
            if (result != Result::None) {
                mView.HandOverHudObjects();
            }
            return result;
        }
        case SlidingMenuAnimation::State::Done:
            mView.HandOverHudObjects();
            return mDeferredResult;
    }
    
    return Result::None;
}

OutOfMovesContinueDialogController::Result OutOfMovesContinueDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

OutOfMovesContinueDialogController::Result
OutOfMovesContinueDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
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
