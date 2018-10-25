#include "PlacePieceWindowController.hpp"

using namespace RowBlast;

PlacePieceWindowController::PlacePieceWindowController(Pht::IEngine& engine,
                                                       const CommonResources& commonResources) :
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void PlacePieceWindowController::Init() {
    mSlidingMenuAnimation.Init(SlidingMenuAnimation::UpdateFade::No,
                               SlidingMenuAnimation::SlideDirection::Scale,
                               SlidingMenuAnimation::UpdatePosition::No);
}

PlacePieceWindowController::Result PlacePieceWindowController::Update() {
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

void PlacePieceWindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
