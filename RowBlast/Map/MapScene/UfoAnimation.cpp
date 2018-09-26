#include "UfoAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Ufo.hpp"
#include "MapScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {2.5f};
    constexpr auto tiltDuration {3.6f};
    constexpr auto maxTiltAngle {4.0f};
    constexpr auto rotationSpeed {100.0f};
    constexpr auto hoverDuration {4.5f};
    constexpr auto maxHoverTranslation {0.095f};
}

UfoAnimation::UfoAnimation(Pht::IEngine& engine, MapScene& scene, Ufo& ufo) :
    mEngine {engine},
    mScene {scene},
    mUfo {ufo} {}

void UfoAnimation::Init() {
    mState = State::Inactive;
    mRotation = {0.0f, 0.0f, 0.0f};
    mElapsedTiltTime = 0.0f;
    mElapsedHoverTime = 0.0f;
}

void UfoAnimation::Start(const Pht::Vec3& destinationPosition) {
    mState = State::Active;
    mStartPosition = mUfo.GetPosition();
    mDestinationPosition = destinationPosition;
    mElapsedTime = 0.0f;
    mScene.SetCameraXPosition(mStartPosition.x);
}

UfoAnimation::State UfoAnimation::Update() {
    UpdateRotation();
    UpdateHoverTranslation();
 
    switch (mState) {
        case State::Active:
            UpdateInActiveState();
            break;
        case State::Finished:
            mState = State::Inactive;
            break;
        case State::Inactive:
            break;
    }
    
    return mState;
}

void UfoAnimation::UpdateRotation() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mRotation.y += dt * rotationSpeed;
    
    if (mRotation.y > 360.0f) {
        mRotation.y -= 360.0f;
    }
    
    mElapsedTiltTime += dt;
    
    if (mElapsedTiltTime > tiltDuration) {
        mElapsedTiltTime -= tiltDuration;
    }
    
    auto normalizedTime {mElapsedTiltTime / tiltDuration};
    mRotation.x = sin(normalizedTime * 2.0f * 3.1415f) * maxTiltAngle;
    mRotation.z = sin(normalizedTime * 2.0f * 3.1415f + 3.1415f) * maxTiltAngle;

    mUfo.SetRotation(mRotation);
}

void UfoAnimation::UpdateHoverTranslation() {
    mElapsedHoverTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedHoverTime > hoverDuration) {
        mElapsedHoverTime -= hoverDuration;
    }
    
    auto normalizedTime {mElapsedHoverTime / hoverDuration};
    auto hoverTranslation {sin(normalizedTime * 2.0f * 3.1415f) * maxHoverTranslation};
    mUfo.SetHoverTranslation(hoverTranslation);
}

void UfoAnimation::UpdateInActiveState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    auto normalizedTime {mElapsedTime / animationDuration};
    auto t {(cos((normalizedTime * 0.5f + 0.5f) * 2.0f * 3.1415f) + 1.0f) / 2.0f};
    
    auto ufoPosition {mStartPosition.Lerp(t, mDestinationPosition)};
    mUfo.SetPosition(ufoPosition);
    
    mScene.SetCameraXPosition(ufoPosition.x);
    
    if (mElapsedTime > animationDuration) {
        mState = State::Finished;
        mUfo.SetPosition(mDestinationPosition);
    }
}

bool UfoAnimation::IsActive() const {
    return mState == State::Active;
}
