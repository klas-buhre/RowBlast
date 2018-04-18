#include "SlidingMenuAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "GuiView.hpp"
#include "UiLayer.hpp"
#include "FadeEffect.hpp"
#include "StaticVector.hpp"
#include "MathUtils.hpp"

using namespace RowBlast;

namespace {
    const auto slideTime {0.3f};
    const Pht::Vec2 centerPosition {0.0f, 0.0f};
    
    Pht::StaticVector<Pht::Vec2, 20> slidePoints {
        {0.0f, 0.0f},
        {0.1f, 0.005f},
        {0.2f, 0.01f},
        {0.3f, 0.02f},
        {0.35f, 0.035f},
        {0.4f, 0.05f},
        {0.45f, 0.065f},
        {0.5f, 0.08f},
        {0.55f, 0.115f},
        {0.6f, 0.15f},
        {0.65f, 0.225f},
        {0.7f, 0.3f},
        {0.75f, 0.41f},
        {0.8f, 0.52f},
        {0.85f, 0.62f},
        {0.9f, 0.7f},
        {0.95f, 0.87f},
        {1.0f, 1.0f},
    };
}

SlidingMenuAnimation::SlidingMenuAnimation(Pht::IEngine& engine, Pht::GuiView& view) :
    mEngine {engine},
    mView {view} {}

void SlidingMenuAnimation::Init(UpdateFade updateFade, SlideDirection slideInDirection) {
    mState = State::Idle;
    mUpdateFade = updateFade;
    mSlideInDirection = slideInDirection;
    
    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    
    switch (mSlideInDirection) {
        case SlideDirection::Up:
            mSlideInStartPosition = {0.0f, -frustumSize.y / 2.0f - mView.GetSize().y / 2.0f};
            break;
        case SlideDirection::Down:
            mSlideInStartPosition = {0.0f, frustumSize.y / 2.0f + mView.GetSize().y / 2.0f};
            break;
    }
    
    mView.SetPosition(mSlideInStartPosition);
    
    if (mUpdateFade == UpdateFade::Yes) {
        mFadeEffect->Reset();
    }
    
    mEngine.GetInput().DisableInput();
}

void SlidingMenuAnimation::StartSlideIn() {
    mView.SetPosition(mSlideInStartPosition);
    
    if (mUpdateFade == UpdateFade::Yes) {
        mFadeEffect->Start();
    }
    
    mState = State::SlidingIn;
    mElapsedTime = 0.0f;
}

void SlidingMenuAnimation::StartSlideOut(UpdateFade updateFade, SlideDirection slideOutDirection) {
    mSlideOutDirection = slideOutDirection;
    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    
    switch (mSlideOutDirection) {
        case SlideDirection::Up:
            mSlideOutFinalPosition = {0.0f, frustumSize.y / 2.0f + mView.GetSize().y / 2.0f};
            break;
        case SlideDirection::Down:
            mSlideOutFinalPosition = {0.0f, -frustumSize.y / 2.0f - mView.GetSize().y / 2.0f};
            break;
    }

    mView.SetPosition(centerPosition);
    
    mUpdateFade = updateFade;
    mState = State::SlidingOut;
    mElapsedTime = 0.0f;
    
    mEngine.GetInput().DisableInput();
}

SlidingMenuAnimation::State SlidingMenuAnimation::Update() {
    switch (mState) {
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState();
            break;
        case State::ShowingMenu:
        case State::Idle:
        case State::Done:
            break;
    }
    
    return mState;
}

void SlidingMenuAnimation::UpdateInSlidingInState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    auto position {mView.GetPosition()};
    auto distance = centerPosition.y - mSlideInStartPosition.y;
    auto normalizedTime {(slideTime - mElapsedTime) / slideTime};
    
    position.y = centerPosition.y - distance * Pht::Lerp(normalizedTime, slidePoints);
    mElapsedTime += dt;
    
    if (mUpdateFade == UpdateFade::Yes &&
        mFadeEffect->GetState() != Pht::FadeEffect::State::Transition) {
        mFadeEffect->Update(dt);
    }
    
    if (HasCompletelySlidIn(position)) {
        position.y = 0.0f;
        
        if (mUpdateFade == UpdateFade::Yes) {
            if (mFadeEffect->GetState() == Pht::FadeEffect::State::Transition) {
                GoToShowingMenuState();
            }
        } else {
            GoToShowingMenuState();
        }
    }
    
    mView.SetPosition(position);
}

bool SlidingMenuAnimation::HasCompletelySlidIn(const Pht::Vec2& position) {
    if (mElapsedTime > slideTime) {
        return true;
    }
    
    switch (mSlideInDirection) {
        case SlideDirection::Up:
            if (position.y >= 0.0f) {
                return true;
            }
            break;
        case SlideDirection::Down:
            if (position.y <= 0.0f) {
                return true;
            }
            break;
    }
    
    return false;
}

void SlidingMenuAnimation::GoToShowingMenuState() {
    mState = State::ShowingMenu;
    mEngine.GetInput().EnableInput();
}

void SlidingMenuAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    auto position {mView.GetPosition()};
    auto distance = mSlideOutFinalPosition.y - centerPosition.y;
    auto normalizedTime {mElapsedTime / slideTime};
    
    position.y = centerPosition.y + distance * normalizedTime * normalizedTime * normalizedTime;
    mElapsedTime += dt;
    
    if (mUpdateFade == UpdateFade::Yes && mFadeEffect->GetState() != Pht::FadeEffect::State::Idle) {
        mFadeEffect->Update(dt);
    }
    
    if (HasCompletelySlidOut(position)) {
        position.y = mSlideOutFinalPosition.y;
        
        if (mUpdateFade == UpdateFade::Yes) {
            if (mFadeEffect->GetState() == Pht::FadeEffect::State::Idle) {
                GoToDoneState();
            }
        } else {
            GoToDoneState();
        }
    }
    
    mView.SetPosition(position);
}

bool SlidingMenuAnimation::HasCompletelySlidOut(const Pht::Vec2& position) {
    if (mElapsedTime > slideTime) {
        return true;
    }
    
    switch (mSlideOutDirection) {
        case SlideDirection::Up:
            if (position.y >= mSlideOutFinalPosition.y) {
                return true;
            }
            break;
        case SlideDirection::Down:
            if (position.y <= mSlideOutFinalPosition.y) {
                return true;
            }
            break;
    }
    
    return false;
}

void SlidingMenuAnimation::GoToDoneState() {
    mState = State::Done;
    mEngine.GetInput().EnableInput();
}
