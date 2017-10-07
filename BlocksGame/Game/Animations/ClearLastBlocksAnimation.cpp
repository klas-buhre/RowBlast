#include "ClearLastBlocksAnimation.hpp"

// Game includes:
#include "FlyingBlocksAnimation.hpp"
#include "Field.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto beforeClearTime {0.5f};
    constexpr auto afterClearTime {1.25f};
}

ClearLastBlocksAnimation::ClearLastBlocksAnimation(Field& field,
                                                   FlyingBlocksAnimation& flyingBlocksAnimation) :
    mField {field},
    mFlyingBlocksAnimation {flyingBlocksAnimation} {}

void ClearLastBlocksAnimation::Start() {
    mElapsedTime = 0.0f;
    mState = State::BeforeClear;
}

ClearLastBlocksAnimation::State ClearLastBlocksAnimation::Update(float dt) {
    switch (mState) {
        case State::BeforeClear:
            UpdateInBeforeClearState(dt);
            break;
        case State::Ongoing:
            UpdateInOngoingState(dt);
            break;
        case State::Inactive:
            break;
    }
    
    return mState;
}

void ClearLastBlocksAnimation::UpdateInBeforeClearState(float dt) {
    mElapsedTime += dt;
    
    if (mElapsedTime > beforeClearTime) {
        auto removedCells {mField.RemoveAllNonEmptySubCells()};
        mFlyingBlocksAnimation.AddBlockRows(removedCells);
        mState = State::Ongoing;
    }
}

void ClearLastBlocksAnimation::UpdateInOngoingState(float dt) {
    mElapsedTime += dt;
    
    if (mElapsedTime > beforeClearTime + afterClearTime) {
        mState = State::Inactive;
    }
}
