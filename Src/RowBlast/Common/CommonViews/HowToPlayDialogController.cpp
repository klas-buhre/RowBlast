#include "HowToPlayDialogController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

HowToPlayDialogController::HowToPlayDialogController(Pht::IEngine& engine,
                                                     const CommonResources& commonResources,
                                                     const PieceResources& pieceResources,
                                                     const LevelResources& levelResources,
                                                     HowToPlayDialogView::SceneId sceneId) :
    mEngine {engine},
    mView {engine, commonResources, pieceResources, levelResources, sceneId},
    mSlidingMenuAnimation {engine, mView},
    mSceneId {sceneId} {}

void HowToPlayDialogController::SetUp() {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::SlideDirection::Left);
}

void HowToPlayDialogController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

void HowToPlayDialogController::SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
    mView.SetGuiLightProvider(guiLightProvider);
}

HowToPlayDialogController::Result HowToPlayDialogController::Update() {
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

HowToPlayDialogController::Result HowToPlayDialogController::HandleInput() {
    return InputUtil::HandleInput<Result>(mEngine.GetInput(),
                                          Result::None,
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

HowToPlayDialogController::Result
HowToPlayDialogController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result::Close;
        
        auto updateFade {
            mSceneId == HowToPlayDialogView::SceneId::Map ? SlidingMenuAnimation::UpdateFade::Yes :
            SlidingMenuAnimation::UpdateFade::No
        };
        
        mSlidingMenuAnimation.StartSlideOut(updateFade,
                                            SlidingMenuAnimation::SlideDirection::Right);
    }
    
    if (mView.GetPreviousButton().IsClicked(touchEvent)) {
        mView.GoToPreviousPage();
    }
    
    if (mView.GetNextButton().IsClicked(touchEvent)) {
        mView.GoToNextPage();
    }
    
    return Result::None;
}
