#include "SwitchPiece2WindowController.hpp"

using namespace RowBlast;

SwitchPiece2WindowController::SwitchPiece2WindowController(Pht::IEngine& engine,
                                                           const CommonResources& commonResources) :
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void SwitchPiece2WindowController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

SwitchPiece2WindowController::Result SwitchPiece2WindowController::Update() {
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

void SwitchPiece2WindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
