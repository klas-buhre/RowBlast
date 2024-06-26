#include "ClearLastBlocksAnimation.hpp"

// Game includes.
#include "FlyingBlocksSystem.hpp"
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto beforeClearTime = 0.1f;
    constexpr auto afterClearTime = 0.25f;
}

ClearLastBlocksAnimation::ClearLastBlocksAnimation(Field& field,
                                                   FlyingBlocksSystem& flyingBlocksSystem) :
    mField {field},
    mFlyingBlocksSystem {flyingBlocksSystem} {}

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
        auto removedCells = mField.RemoveAllNonEmptySubCells();
        mFlyingBlocksSystem.AddBlocks(removedCells);
        mState = State::Ongoing;
    }
}

void ClearLastBlocksAnimation::UpdateInOngoingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > beforeClearTime + afterClearTime) {
        mState = State::Inactive;
    }
}
