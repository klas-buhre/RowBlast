#include "LevelGoalDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

LevelGoalDialogController::LevelGoalDialogController(Pht::IEngine& engine,
                                                     const CommonResources& commonResources,
                                                     const PieceResources& pieceResources,
                                                     LevelGoalDialogView::Scene scene) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, pieceResources, scene},
    mSlidingMenuAnimation {engine, mView},
    mScene {scene} {}

void LevelGoalDialogController::Init(const LevelInfo& levelInfo) {
    mView.Init(levelInfo);
    
    switch (mScene) {
        case LevelGoalDialogView::Scene::Map:
            mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::Yes,
                                       SlidingMenuAnimation::SlideDirection::Scale);
            break;
        case LevelGoalDialogView::Scene::Game:
            mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::No,
                                       SlidingMenuAnimation::SlideDirection::Left);
            break;
    }
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
    switch (mScene) {
        case LevelGoalDialogView::Scene::Map:
            return OnTouchInMapScene(touchEvent);
        case LevelGoalDialogView::Scene::Game:
            return OnTouchInGameScene(touchEvent);
    }
}

LevelGoalDialogController::Result
LevelGoalDialogController::OnTouchInMapScene(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Left);
        return Result::None;
    }

    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        return Result::Play;
    }
    
    return Result::None;
}

LevelGoalDialogController::Result
LevelGoalDialogController::OnTouchInGameScene(const Pht::TouchEvent& touchEvent) {
    if (mView.GetBackButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    return Result::None;
}
