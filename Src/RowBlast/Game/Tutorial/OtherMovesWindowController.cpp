#include "OtherMovesWindowController.hpp"

using namespace RowBlast;

OtherMovesWindowController::OtherMovesWindowController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources) :
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OtherMovesWindowController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

OtherMovesWindowController::Result OtherMovesWindowController::Update() {
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::Done:
            return Result::Done;
        default:
            break;
    }
    
    return Result::None;
}

void OtherMovesWindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
