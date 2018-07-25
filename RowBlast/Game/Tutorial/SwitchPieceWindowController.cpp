#include "SwitchPieceWindowController.hpp"

using namespace RowBlast;

SwitchPieceWindowController::SwitchPieceWindowController(Pht::IEngine& engine,
                                                         const CommonResources& commonResources) :
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void SwitchPieceWindowController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Scale,
                               SlidingMenuAnimation::UpdatePosition::No);
}

SwitchPieceWindowController::Result SwitchPieceWindowController::Update() {
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

void SwitchPieceWindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
