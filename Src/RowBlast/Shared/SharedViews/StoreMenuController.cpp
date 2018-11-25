#include "StoreMenuController.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "InputUtil.hpp"

using namespace RowBlast;

StoreMenuController::Result::Result() :
    mKind {None} {}

StoreMenuController::Result::Result(Kind kind) :
    mKind {kind} {}

StoreMenuController::Result::Result(ProductId productId) :
    mKind {PurchaseProduct},
    mProductId {productId} {}

bool StoreMenuController::Result::operator==(const Result& other) const {
    if (mKind != other.mKind) {
        return false;
    }
    
    if (mKind == PurchaseProduct) {
        if (mProductId != other.mProductId) {
            return false;
        }
    }
    
    return true;
}

bool StoreMenuController::Result::operator!=(const Result& other) const {
    return !(*this == other);
}

ProductId StoreMenuController::Result::GetProductId() const {
    assert(mKind == PurchaseProduct);
    return mProductId;
}

StoreMenuController::StoreMenuController(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const UserServices& userServices,
                                         PotentiallyZoomedScreen potentiallyZoomedScreen) :
    mInput {engine.GetInput()},
    mView {engine, commonResources, userServices, potentiallyZoomedScreen},
    mSlidingMenuAnimation {engine, mView} {}

void StoreMenuController::SetUp(SlidingMenuAnimation::UpdateFade updateFade,
                                SlidingMenuAnimation::SlideDirection slideDirection) {
    mView.SetUp();
    mSlidingMenuAnimation.SetUp(updateFade, slideDirection);
}

void StoreMenuController::SetFadeEffect(Pht::FadeEffect& fadeEffect) {
    mSlidingMenuAnimation.SetFadeEffect(fadeEffect);
}

StoreMenuController::Result StoreMenuController::Update() {
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
    
    return Result {Result::None};
}

StoreMenuController::Result StoreMenuController::HandleInput() {
    return InputUtil::HandleInput<Result>(mInput,
                                          Result {Result::None},
                                          [this] (const Pht::TouchEvent& touch) {
                                              return OnTouch(touch);
                                          });
}

StoreMenuController::Result StoreMenuController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mView.GetCloseButton().IsClicked(touchEvent)) {
        mDeferredResult = Result {Result::Close};
        mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::Yes,
                                            SlidingMenuAnimation::SlideDirection::Right);
        return Result {Result::None};
    }
    
    for (auto& productSection: mView.GetProductSections()) {
        if (productSection.mPurchaseButton->IsClicked(touchEvent)) {
            mDeferredResult = Result {productSection.mProductId};
            mSlidingMenuAnimation.StartSlideOut(SlidingMenuAnimation::UpdateFade::No,
                                                SlidingMenuAnimation::SlideDirection::Left);
            return Result {Result::None};
        }
    }
    
    return Result {Result::None};
}
