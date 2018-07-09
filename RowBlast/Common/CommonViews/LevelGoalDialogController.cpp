#include "LevelGoalDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

LevelGoalDialogController::LevelGoalDialogController(Pht::IEngine& engine,
                                                     const CommonResources& commonResources,
                                                     PieceResources& pieceResources,
                                                     PotentiallyZoomedScreen zoom) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, pieceResources, zoom},
    mSlidingMenuAnimation {engine, mView} {}

void LevelGoalDialogController::Init(SlidingMenuAnimation::UpdateFade updateFade,
                                     const LevelInfo& levelInfo) {
    mUpdateFade = updateFade;
    
    auto slideInDirection {
        updateFade == SlidingMenuAnimation::UpdateFade::Yes ?
            SlidingMenuAnimation::SlideDirection::Scale :
            SlidingMenuAnimation::SlideDirection::Left
    };

    mView.Init(levelInfo);
    mSlidingMenuAnimation.Init(updateFade, slideInDirection);
}

void LevelGoalDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

LevelGoalDialogController::Result LevelGoalDialogController::Update() {
    mView.Update();
    
    auto previousSlidingMenuAnimationState {mSlidingMenuAnimation.GetState()};
    
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            if (previousSlidingMenuAnimationState != SlidingMenuAnimation::State::ShowingMenu) {
                mView.StartEffects();
            }
            return HandleInput();
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

LevelGoalDialogController::Result LevelGoalDialogController::HandleInput() {
    return InputUtil::HandleInput<Result, Result::None>(
        mInput, [this] (const Pht::TouchEvent& touch) { return OnTouch(touch); });
}

LevelGoalDialogController::Result
LevelGoalDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(mUpdateFade,
                                            SlidingMenuAnimation::SlideDirection::Left);
    }

    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        return Result::Play;
    }
    
    return Result::None;
}
