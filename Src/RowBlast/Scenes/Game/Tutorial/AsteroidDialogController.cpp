#include "AsteroidDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

AsteroidDialogController::AsteroidDialogController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources,
                                                   const PieceResources& pieceResources,
                                                   const GhostPieceBlocks& ghostPieceBlocks,
                                                   const LevelResources& levelResources,
                                                   const UserServices& userServices) :
    mInput {engine.GetInput()},
    mView {
        engine,
        commonResources,
        pieceResources,
        ghostPieceBlocks,
        levelResources,
        userServices
    },
    mSlidingMenuAnimation {engine, mView} {}

void AsteroidDialogController::SetUp() {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::SlideDirection::Scale,
                                SlidingMenuAnimation::UpdatePosition::No);
}

void AsteroidDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void AsteroidDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

AsteroidDialogController::Result AsteroidDialogController::Update() {
    mView.Update();

    switch (mSlidingMenuAnimation.Update()) {
        case SlidingMenuAnimation::State::Idle:
            mSlidingMenuAnimation.StartSlideIn();
            break;
        case SlidingMenuAnimation::State::SlidingIn:
        case SlidingMenuAnimation::State::SlidingOut:
            break;
        case SlidingMenuAnimation::State::ShowingMenu:
            return HandleInput();
        case SlidingMenuAnimation::State::Done:
            return mDeferredResult;
    }
    
    return Result::None;
}

AsteroidDialogController::Result AsteroidDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

AsteroidDialogController::Result AsteroidDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetPlayButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Play;
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Scale,
                                            SlidingMenuAnimation::UpdatePosition::No);
    }
    
    return Result::None;
}
