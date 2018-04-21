#include "CameraShake.hpp"

#include "MathUtils.hpp"

using namespace Pht;

void CameraShake::Init() {
    mState = State::Inactive;
    mCameraTranslation = {0.0f, 0.0f, 0.0f};
}

void CameraShake::Update(float dt) {
    if (mState == State::Inactive) {
        return;
    }
    
    auto magnitude {mMagnitude * (mShakeTime - mElapsedTime) / mShakeTime};
    
    mCameraTranslation.x = magnitude * (NormalizedRand() - 0.5f);
    mCameraTranslation.y = magnitude * (NormalizedRand() - 0.5f);
    mCameraTranslation.z = 0.0f;
    
    mElapsedTime += dt;
    
    if (mElapsedTime >= mShakeTime) {
        mState = State::Inactive;
        mCameraTranslation = {0.0f, 0.0f, 0.0f};
    }
}

void CameraShake::StartShake(float shakeTime, float magnitude) {
    mState = State::Shaking;
    mElapsedTime = 0.0f;
    mShakeTime = shakeTime;
    mMagnitude = magnitude;
}
