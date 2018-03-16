#include "CollapsingFieldAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto gravitationalAcceleration {-35.0f};
    constexpr auto waitTime {0.15f};
    constexpr auto fixedTimeStep {0.005f};

    void UpdateBlockInBouncingState(SubCell& subCell, int row, float dt) {
        auto springBoundry {static_cast<float>(row)};
        auto& blockPosition {subCell.mPosition};
        auto& animation {subCell.mFallingBlockAnimation};
        auto springDisplacement {springBoundry - blockPosition.y};
        auto acceleration {2000.0f * springDisplacement + gravitationalAcceleration};
        
        animation.mVelocity += acceleration * dt;
        blockPosition.y += animation.mVelocity * dt;
        
        if (blockPosition.y > row) {
            blockPosition.y = row;
            animation = FallingBlockAnimation {};
        }
    }

    void UpdateBlockInFallingState(SubCell& subCell, int row, float dt) {
        auto& blockPosition {subCell.mPosition};
        auto& animation {subCell.mFallingBlockAnimation};
        
        animation.mVelocity += gravitationalAcceleration * dt;
        blockPosition.y += animation.mVelocity * dt;
    
        if (blockPosition.y < row) {
            animation.mState = FallingBlockAnimation::State::Bouncing;
        }
    }

    /*
    void UpdateBlockInFallingState(SubCell& subCell, int row, float dt) {
        auto& blockPosition {subCell.mPosition};
        auto& animation {subCell.mFallingBlockAnimation};
        auto newVelocity {gravitationalAcceleration * dt + animation.mVelocity};
        auto dy {newVelocity * dt};
        auto newYPosition {blockPosition.y + dy};
        
        if (newYPosition < row) {
            animation.mState = FallingBlockAnimation::State::Bouncing;
            
            auto springBoundry {static_cast<float>(row)};
            auto frameTimeNotTouchingSpring {dt * (blockPosition.y - springBoundry) / dy};
            animation.mVelocity += gravitationalAcceleration * frameTimeNotTouchingSpring;
            
            auto frameTimeTouchingSpring {dt * (springBoundry - newYPosition) / dy};
            UpdateBlockInBouncingState(subCell, row, frameTimeTouchingSpring);
            
            blockPosition.y = row;
        } else {
            animation.mVelocity = newVelocity;
            blockPosition.y = newYPosition;
        }
    }
    */

    FallingBlockAnimation::State UpdateBlockFixedTimeStep(SubCell& subCell, int row, float dt) {
        switch (subCell.mFallingBlockAnimation.mState) {
            case FallingBlockAnimation::State::Falling:
                UpdateBlockInFallingState(subCell, row, dt);
                break;
            case FallingBlockAnimation::State::Bouncing:
                UpdateBlockInBouncingState(subCell, row, dt);
                break;
            case FallingBlockAnimation::State::Inactive:
                if (subCell.mPosition.y > row) {
                    subCell.mFallingBlockAnimation.mState = FallingBlockAnimation::State::Falling;
                }
                break;
        }
        
        return subCell.mFallingBlockAnimation.mState;
    }

    FallingBlockAnimation::State UpdateBlock(SubCell& subCell, int row, float frameDuration) {
        if (frameDuration <= fixedTimeStep) {
            return UpdateBlockFixedTimeStep(subCell, row, frameDuration);
        }
        
        FallingBlockAnimation::State blockAnimationstate {FallingBlockAnimation::State::Inactive};
        auto frameTimeLeft {frameDuration};
        
        for (;;) {
            auto timeStep {frameTimeLeft > fixedTimeStep ? fixedTimeStep : frameTimeLeft};
            
            blockAnimationstate = UpdateBlockFixedTimeStep(subCell, row, timeStep);
            frameTimeLeft -= fixedTimeStep;
            
            if (frameTimeLeft <= 0.0f ||
                blockAnimationstate == FallingBlockAnimation::State::Inactive) {
                break;
            }
        }
        
        return blockAnimationstate;
    }
}

CollapsingFieldAnimation::CollapsingFieldAnimation(Field& field) :
    mField {field} {}

CollapsingFieldAnimation::State CollapsingFieldAnimation::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState(dt);
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            UpdateInInactiveState();
            break;
    }
    
    return mState;
}

void CollapsingFieldAnimation::UpdateInWaitingState(float dt) {
    mWaitedTime += dt;

    if (mWaitedTime > waitTime) {
        mState = State::Active;
    }
}

void CollapsingFieldAnimation::UpdateInActiveState(float dt) {
    auto anyMovingCells {false};
    
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            if (UpdateBlock(cell.mFirstSubCell, row, dt) != FallingBlockAnimation::State::Inactive ||
                UpdateBlock(cell.mSecondSubCell, row, dt) != FallingBlockAnimation::State::Inactive) {
                anyMovingCells = true;
            }
        }
    }
    
    mField.SetChanged();
    
    if (!anyMovingCells) {
        mState = State::Inactive;
    }
}

void CollapsingFieldAnimation::UpdateInInactiveState() {
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            if (cell.mFirstSubCell.mPosition.y > row || cell.mSecondSubCell.mPosition.y > row) {
                GoToWaitingState();
                return;
            }
        }
    }
}

void CollapsingFieldAnimation::GoToWaitingState() {
    mWaitedTime = 0.0f;
    mState = State::Waiting;
}
