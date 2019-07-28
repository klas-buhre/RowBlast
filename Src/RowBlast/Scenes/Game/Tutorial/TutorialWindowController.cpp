#include "TutorialWindowController.hpp"

using namespace RowBlast;

TutorialWindowController::TutorialWindowController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources,
                                                   const std::vector<std::string>& textLines,
                                                   float yPosition) :
    mView {commonResources, textLines, yPosition},
    mSlidingMenuAnimation {engine, mView} {}

void TutorialWindowController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

TutorialWindowController::Result TutorialWindowController::Update() {
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

void TutorialWindowController::Close() {
    mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                        SlidingMenuAnimation::SlideDirection::Scale,
                                        SlidingMenuAnimation::UpdatePosition::No);
}
