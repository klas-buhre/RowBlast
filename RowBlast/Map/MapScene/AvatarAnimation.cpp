#include "AvatarAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Avatar.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {2.0f};
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
    if (mState == State::Inactive) {
        return mState;
    }
    
    mElapsedTime += mEngine.GetLastFrameSeconds();
    auto normalizedTime {mElapsedTime / animationDuration};

    auto avatarPosition {mStartPosition.Lerp(normalizedTime, mDestinationPosition)};
    mAvatar.SetPosition(avatarPosition);
    
    if (mElapsedTime > animationDuration) {
        mState = State::Inactive;
        mAvatar.SetPosition(mDestinationPosition);
    }
    
    return mState;
}
