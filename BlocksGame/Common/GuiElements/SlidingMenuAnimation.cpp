#include "SlidingMenuAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "GuiView.hpp"

using namespace BlocksGame;

namespace {
    const auto slideTime {0.3f};
    const Pht::Vec2 centerPosition {0.0f, 0.0f};
}

SlidingMenuAnimation::SlidingMenuAnimation(Pht::IEngine& engine, Pht::GuiView& view, float fade) :
    mEngine {engine},
    mView {view},
    mFadeEffect {engine, slideTime, fade} {
    
    auto& frustumSize {engine.GetRenderer().GetHudFrustumSize()};
    mSlideInStartPosition = {0.0f, -frustumSize.y / 2.0f - view.GetSize().y / 2.0f};
    mSlideOutFinalPosition = {0.0f, frustumSize.y / 2.0f + view.GetSize().y / 2.0f};
}

void SlidingMenuAnimation::Reset() {
    mState = State::Idle;
    mView.SetPosition(mSlideInStartPosition);
    mFadeEffect.Reset();
}

void SlidingMenuAnimation::StartSlideIn() {
    auto distance = centerPosition - mSlideInStartPosition;
    mVelocity = distance * 2.0f / slideTime;
    mAcceleration = -mVelocity / slideTime;

    mView.SetPosition(mSlideInStartPosition);
    mFadeEffect.Start();
    
    mState = State::SlidingIn;
    mElapsedTime = 0.0f;
}

void SlidingMenuAnimation::StartSlideOut() {
    auto distance = mSlideOutFinalPosition - centerPosition;
    auto finalVelocity {distance * 2.0f / slideTime};
    mVelocity = {0.0f, 0.0f};
    mAcceleration = finalVelocity / slideTime;

    mView.SetPosition(centerPosition);
    
    mState = State::SlidingOut;
    mElapsedTime = 0.0f;
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
    position += mVelocity * dt;
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    if (mFadeEffect.GetState() != Pht::FadeEffect::State::Transition) {
        mFadeEffect.Update();
    }
    
    if (position.y >= 0.0f || mElapsedTime > slideTime) {
        position.y = 0.0f;
        
        if (mFadeEffect.GetState() == Pht::FadeEffect::State::Transition) {
            mState = State::ShowingMenu;
        }
    }
    
    mView.SetPosition(position);
}

void SlidingMenuAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    auto position {mView.GetPosition()};
    position += mVelocity * dt;
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    if (mFadeEffect.GetState() != Pht::FadeEffect::State::Idle) {
        mFadeEffect.Update();
    }
    
    if (position.y >= mSlideOutFinalPosition.y || mElapsedTime > slideTime) {
        position.y = mSlideOutFinalPosition.y;
        
        if (mFadeEffect.GetState() == Pht::FadeEffect::State::Idle) {
            mState = State::Done;
        }
    }
    
    mView.SetPosition(position);
}
