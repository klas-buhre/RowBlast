#include "SlidingTextAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto slideTime {0.5f};
    const Pht::Vec2 centerPosition {0.0f, 0.0f};
    constexpr auto textHeight {3.0f};
    constexpr auto alpha {0.88f};

    const std::vector<SlidingTextAnimation::Text> texts {
        {
            2.7f,
            {
                {Pht::Vec2{-3.1f, 1.0f}, "Clear all"},
                {Pht::Vec2{-4.0f, -1.0f}, "gray blocks!"}
            }
        },
        {
            2.7f,
            {
                {Pht::Vec2{-4.0f, 1.0f}, "You cleared"},
                {Pht::Vec2{-3.55f, -1.0f}, "all blocks!"}
            }
        },
        {
           2.7f,
            {
                {Pht::Vec2{-2.5f, 1.0f}, "Fill all"},
                {Pht::Vec2{-3.5f, -1.0f}, "gray slots!"}
            }
        },
        {
            2.7f,
            {
                {Pht::Vec2{-3.3f, 1.0f}, "You filled"},
                {Pht::Vec2{-3.05f, -1.0f}, "all slots!"}
            }
        }
    };
}

SlidingTextAnimation::SlidingTextAnimation(Pht::IEngine& engine) :
    mEngine {engine},
    mFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(52)},
    mTextProperties {mFont} {

    auto& frustumSize {engine.GetRenderer().GetHudFrustumSize()};
    mSlideInStartPosition = {0.0f, -frustumSize.y / 4.0f - textHeight / 2.0f};
    mSlideOutFinalPosition = {0.0f, frustumSize.y / 4.0f + textHeight / 2.0f};
}

void SlidingTextAnimation::Start(Message message) {
    mState = State::SlidingIn;
    mText = &texts[static_cast<int>(message)];

    auto distance = centerPosition - mSlideInStartPosition;
    mVelocity = distance * 2.0f / slideTime;
    mAcceleration = -mVelocity / slideTime;
    mPosition = mSlideInStartPosition;
    mElapsedTime = 0.0f;
    mTextProperties.mColor.w = 0.0f;
}

SlidingTextAnimation::State SlidingTextAnimation::Update() {
    switch (mState) {
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::DisplayingText:
            UpdateInDisplayingTextState();
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState();
            break;
        case State::Inactive:
            break;
    }
    
    return mState;    
}

void SlidingTextAnimation::UpdateInSlidingInState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mPosition += mVelocity * dt;
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    mTextProperties.mColor.w = alpha * mElapsedTime / slideTime;
    
    if (mPosition.y >= 0.0f || mElapsedTime > slideTime) {
        mPosition.y = 0.0f;
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        mTextProperties.mColor.w = alpha;
    }
}

void SlidingTextAnimation::UpdateInDisplayingTextState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime > mText->mDisplayTime || mEngine.GetInput().ConsumeWholeTouch()) {
        StartSlideOut();
    }
}

void SlidingTextAnimation::StartSlideOut() {
    mState = State::SlidingOut;
    
    auto distance = mSlideOutFinalPosition - centerPosition;
    auto finalVelocity {distance * 2.0f / slideTime};
    mVelocity = {0.0f, 0.0f};
    mAcceleration = finalVelocity / slideTime;
    mElapsedTime = 0.0f;
}

void SlidingTextAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mPosition += mVelocity * dt;
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    mTextProperties.mColor.w = alpha * (slideTime - mElapsedTime) / slideTime;
    
    if (mPosition.y >= mSlideOutFinalPosition.y || mElapsedTime > slideTime) {
        mPosition.y = mSlideOutFinalPosition.y;
        mState = State::Inactive;
    }
}
