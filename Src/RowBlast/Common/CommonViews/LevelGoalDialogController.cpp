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
                                                     LevelGoalDialogView::SceneId sceneId) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, pieceResources, sceneId},
    mSlidingMenuAnimation {engine, mView},
    mSceneId {sceneId} {}

void LevelGoalDialogController::SetUp(const LevelInfo& levelInfo) {
    mView.SetUp(levelInfo);
    
    switch (mSceneId) {
        case LevelGoalDialogView::SceneId::Map:
            mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                        SlidingMenuAnimation::SlideDirection::Scale);
            break;
        case LevelGoalDialogView::SceneId::Game:
            mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
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
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

LevelGoalDialogController::Result
LevelGoalDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    switch (mSceneId) {
        case LevelGoalDialogView::SceneId::Map:
            return OnTouchInMapScene(touchEvent);
        case LevelGoalDialogView::SceneId::Game:
            return OnTouchInGameScene(touchEvent);
    }
}

LevelGoalDialogController::Result
LevelGoalDialogController::OnTouchInMapScene(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
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
