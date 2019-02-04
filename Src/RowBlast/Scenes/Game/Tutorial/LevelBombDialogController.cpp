#include "LevelBombDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

LevelBombDialogController::LevelBombDialogController(Pht::IEngine& engine,
                                                     const CommonResources& commonResources,
                                                     const UserServices& userServices) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, userServices},
    mSlidingMenuAnimation {engine, mView} {}

void LevelBombDialogController::SetUp(Pht::Scene& scene) {
    mView.SetUp(scene);
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

void LevelBombDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

LevelBombDialogController::Result LevelBombDialogController::Update() {
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

LevelBombDialogController::Result LevelBombDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

LevelBombDialogController::Result LevelBombDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Play;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Scale,
                                            SlidingMenuAnimation::UpdatePosition::No);
    }
    
    return Result::None;
}
