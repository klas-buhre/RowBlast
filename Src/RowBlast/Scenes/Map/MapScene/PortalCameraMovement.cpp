#include "PortalCameraMovement.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto duration = 0.9f;
    constexpr auto targetZ = -300.0f;
    constexpr auto fadeDuration = 0.22f;
}

PortalCameraMovement::PortalCameraMovement(Pht::IEngine& engine, MapScene& scene) :
    mEngine {engine},
    mScene {scene} {}

void PortalCameraMovement::Start() {
    mState = State::Active;
    mHasBeenInStartFadeOutState = false;
    mElapsedTime = 0.0f;
    mStartZ = mScene.GetCameraZPosition();
    mScene.HideAllLevelPinTexts();
}

PortalCameraMovement::State PortalCameraMovement::Update() {
    switch (mState) {
        case State::Inactive:
            break;
        case State::Active:
        case State::StartFadeOut:
            UpdateInActiveState();
            break;
    }
    
    return mState;
}

void PortalCameraMovement::UpdateInActiveState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mElapsedTime += dt;
    if (mElapsedTime >= duration) {
        if (!mHasBeenInStartFadeOutState) {
            mState = State::StartFadeOut;
            mHasBeenInStartFadeOutState = true;
        } else {
            mState = State::Inactive;
        }

        return;
    }
    
    if (mElapsedTime >= duration - fadeDuration) {
        if (!mHasBeenInStartFadeOutState) {
            mState = State::StartFadeOut;
            mHasBeenInStartFadeOutState = true;
        } else {
            mState = State::Active;
        }
    }
    
    auto t = mElapsedTime / duration;
    auto cameraZPosition = mStartZ + (targetZ - mStartZ) * t * t;
    mScene.SetCameraZPosition(cameraZPosition);
}
