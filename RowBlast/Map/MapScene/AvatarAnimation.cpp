#include "AvatarAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Avatar.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {2.8f};
}

AvatarAnimation::AvatarAnimation(Pht::IEngine& engine, Avatar& avatar) :
    mEngine {engine},
    mAvatar {avatar} {}

void AvatarAnimation::Init() {
    mState = State::Inactive;
}

void AvatarAnimation::Start(const Pht::Vec3& destinationPosition) {
    mState = State::Active;
    mStartPosition = mAvatar.GetPosition();
    mDestinationPosition = destinationPosition;
    mElapsedTime = 0.0f;
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
    
    if (mElapsedTime > animationDuration) {
        mState = State::Finished;
        mAvatar.SetPosition(mDestinationPosition);
    }
}
