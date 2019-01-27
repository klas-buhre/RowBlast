#include "OtherMoves2WindowController.hpp"

using namespace RowBlast;

OtherMoves2WindowController::OtherMoves2WindowController(Pht::IEngine& engine,
                                                         const CommonResources& commonResources) :
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void OtherMoves2WindowController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

OtherMoves2WindowController::Result OtherMoves2WindowController::Update() {
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

void OtherMoves2WindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
