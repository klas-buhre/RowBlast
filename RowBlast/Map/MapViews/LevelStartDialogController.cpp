#include "LevelStartDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

LevelStartDialogController::LevelStartDialogController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources) :
    mInput {engine.GetInput()},
    mView {engine, commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void LevelStartDialogController::Init(const LevelInfo& levelInfo,
                                      const PieceResources& pieceResources) {
    mView.Init(levelInfo, pieceResources);
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::Yes,
                               SlidingMenuAnimation::SlideDirection::Scale);
}

void LevelStartDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

LevelStartDialogController::Result LevelStartDialogController::Update() {
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

LevelStartDialogController::Result LevelStartDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

LevelStartDialogController::Result
LevelStartDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Left);
    }

    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        return Result::Play;
    }
    
    return Result::None;
}
