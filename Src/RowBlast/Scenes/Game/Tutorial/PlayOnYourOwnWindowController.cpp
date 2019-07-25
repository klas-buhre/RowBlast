#include "PlayOnYourOwnWindowController.hpp"

// Engine includes.
#include "IEngine.hpp"

using namespace RowBlast;

namespace {
    constexpr auto displayTime = 5.0f;
}

PlayOnYourOwnWindowController::PlayOnYourOwnWindowController(Pht::IEngine& engine,
                                                             const CommonResources& commonResources) :
    mEngine {engine},
    mView {commonResources},
    mSlidingMenuAnimation {engine, mView} {}

void PlayOnYourOwnWindowController::Init() {
    mShownNumTimes = 0;
    mElapsedTime = 0.0f;
    mIsGamePaused = false;
}

void PlayOnYourOwnWindowController::SetUp() {
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
    ++mShownNumTimes;
}

void PlayOnYourOwnWindowController::OnPause() {
    mIsGamePaused = true;
}

void PlayOnYourOwnWindowController::OnResumePlaying() {
    mIsGamePaused = false;
}

PlayOnYourOwnWindowController::Result PlayOnYourOwnWindowController::Update() {
    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            if (!mIsGamePaused) {
                mElapsedTime += mEngine.GetLastFrameSeconds();
                if (mElapsedTime >= displayTime) {
                    Close();
                }
            }
            break;
        case SlidingMenuAnimation::State::Done:
            return Result::Done;
        default:
            break;
    }
    
    return Result::None;
}

void PlayOnYourOwnWindowController::Close() {
    if (mSlidingMenuAnimation.GetState() == SlidingMenuAnimation::State::ShowingMenu) {
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                            SlidingMenuAnimation::SlideDirection::Scale,
                                            SlidingMenuAnimation::UpdatePosition::No);
    }
}
