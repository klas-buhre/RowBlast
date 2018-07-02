#include "AvatarAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Avatar.hpp"
#include "MapScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {2.5f};
}

AvatarAnimation::AvatarAnimation(Pht::IEngine& engine, MapScene& scene, Avatar& avatar) :
    mEngine {engine},
    mScene {scene},
    mAvatar {avatar} {}

void AvatarAnimation::Init() {
    mState = State::Inactive;
}

void AvatarAnimation::Start(const Pht::Vec3& destinationPosition) {
    mState = State::Active;
    mStartPosition = mAvatar.GetPosition();
    mDestinationPosition = destinationPosition;
    mElapsedTime = 0.0f;
    mScene.SetCameraXPosition(mStartPosition.x);
}

AvatarAnimation::State AvatarAnimation::Update() {
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

void AvatarAnimation::UpdateInActiveState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    auto normalizedTime {mElapsedTime / animationDuration};
    auto t {(cos((normalizedTime * 0.5f + 0.5f) * 2.0f * 3.1415f) + 1.0f) / 2.0f};
    
    auto avatarPosition {mStartPosition.Lerp(t, mDestinationPosition)};
    mAvatar.SetPosition(avatarPosition);
    
    mScene.SetCameraXPosition(avatarPosition.x);
    
    if (mElapsedTime > animationDuration) {
        mState = State::Finished;
        mAvatar.SetPosition(mDestinationPosition);
    }
}

bool AvatarAnimation::IsActive() const {
    return mState == State::Active;
}
