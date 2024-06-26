#include "CollapsingFieldAnimationSystem.hpp"

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto gravitationalAcceleration = -55.0f;
    constexpr auto springCoefficient = 5000.0f;
    constexpr auto dampingCoefficient = 55.75f;
    constexpr auto waitTime = 0.18f;
    constexpr auto fallingStateFixedTimeStep = 0.016f;
    constexpr auto bouncingStateFixedTimeStep = 0.002f;
    
    void UpdateBlockWhileTouchingSpring(SubCell& subCell, int row, float dt) {
        auto springBoundry = static_cast<float>(row);
        auto& blockPosition = subCell.mPosition;
        auto& animation = subCell.mFallingBlockAnimation;
        auto springDisplacement = springBoundry - blockPosition.y;
        
        auto acceleration =
            springCoefficient * springDisplacement + gravitationalAcceleration -
            dampingCoefficient * animation.mVelocity;
        
        auto previousVelocity = animation.mVelocity;
        
        animation.mVelocity += acceleration * dt;
        blockPosition.y += animation.mVelocity * dt;
        
        if (previousVelocity >= 0.0f && animation.mVelocity <= 0.0f &&
            blockPosition.y <= springBoundry) {
            
            blockPosition.y = row;
            animation = FallingBlockAnimationComponent {};
        }
    }

    void UpdateBlockInBouncingStateFixedTimeStep(SubCell& subCell, int row, float dt) {
        auto springBoundry = static_cast<float>(row);
        auto& blockPosition = subCell.mPosition;

        if (blockPosition.y <= springBoundry) {
            UpdateBlockWhileTouchingSpring(subCell, row, dt);
        } else {
            auto& animation = subCell.mFallingBlockAnimation;
            
            animation.mVelocity += gravitationalAcceleration * dt;
            blockPosition.y += animation.mVelocity * dt;
        
            if (blockPosition.y < row) {
                blockPosition.y = row;
                animation = FallingBlockAnimationComponent {};
            }
        }
    }
    
    void UpdateBlockInBouncingState(SubCell& subCell, int row, float frameDuration) {
        if (frameDuration <= bouncingStateFixedTimeStep) {
            UpdateBlockInBouncingStateFixedTimeStep(subCell, row, frameDuration);
            return;
        }
        
        auto frameTimeLeft = frameDuration;
        
        for (;;) {
            auto timeStep =
                frameTimeLeft > bouncingStateFixedTimeStep ? bouncingStateFixedTimeStep :
                frameTimeLeft;
            
            UpdateBlockInBouncingStateFixedTimeStep(subCell, row, timeStep);
            frameTimeLeft -= bouncingStateFixedTimeStep;
            
            if (frameTimeLeft <= 0.0f ||
                subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Inactive) {
                break;
            }
        }
    }

    void UpdateBlockInFallingStateFixedTimeStep(SubCell& subCell, int row, float dt) {
        auto& blockPosition = subCell.mPosition;
        auto& animation = subCell.mFallingBlockAnimation;
        auto newVelocity = gravitationalAcceleration * dt + animation.mVelocity;
        auto dy = newVelocity * dt;
        auto newYPosition = blockPosition.y + dy;
        
        if (newYPosition < row) {
            if (animation.mShouldBounce) {
                animation.mState = FallingBlockAnimationComponent::State::Bouncing;
                
                auto springBoundry = static_cast<float>(row);
                auto frameTimeNotTouchingSpring = dt * (blockPosition.y - springBoundry) / -dy;
                animation.mVelocity += gravitationalAcceleration * frameTimeNotTouchingSpring;
                
                auto frameTimeTouchingSpring = dt * (springBoundry - newYPosition) / -dy;
                blockPosition.y = row;
                UpdateBlockInBouncingState(subCell, row, frameTimeTouchingSpring);
            } else {
                blockPosition.y = row;
                animation = FallingBlockAnimationComponent {};
            }
        } else {
            animation.mVelocity = newVelocity;
            blockPosition.y = newYPosition;
        }
    }

    void UpdateBlockInFallingState(SubCell& subCell, int row, float frameDuration) {
        if (frameDuration <= fallingStateFixedTimeStep) {
            UpdateBlockInFallingStateFixedTimeStep(subCell, row, frameDuration);
            return;
        }
        
        auto frameTimeLeft = frameDuration;
        
        for (;;) {
            auto timeStep =
                frameTimeLeft > fallingStateFixedTimeStep ? fallingStateFixedTimeStep :
                frameTimeLeft;
            
            UpdateBlockInFallingStateFixedTimeStep(subCell, row, timeStep);
            frameTimeLeft -= fallingStateFixedTimeStep;
            
            if (frameTimeLeft <= 0.0f ||
                subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Inactive) {
                break;
            } else if (subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing) {
                UpdateBlockInBouncingState(subCell, row, frameTimeLeft);
                break;
            }
        }
    }

    FallingBlockAnimationComponent::State UpdateBlock(SubCell& subCell, int row, float dt) {
        switch (subCell.mFallingBlockAnimation.mState) {
            case FallingBlockAnimationComponent::State::Falling:
                UpdateBlockInFallingState(subCell, row, dt);
                break;
            case FallingBlockAnimationComponent::State::Bouncing:
                UpdateBlockInBouncingState(subCell, row, dt);
                break;
            case FallingBlockAnimationComponent::State::Inactive:
                if (subCell.mPosition.y > row) {
                    subCell.mFallingBlockAnimation.mState = FallingBlockAnimationComponent::State::Falling;
                }
                break;
        }
        
        return subCell.mFallingBlockAnimation.mState;
    }
    
    void TransitionWronglyBouncingBlockToFalling(SubCell& subCell, int row) {
        auto& animation = subCell.mFallingBlockAnimation;
        
        if (animation.mState == FallingBlockAnimationComponent::State::Bouncing &&
            animation.mVelocity == FallingBlockAnimationComponent::fallingPieceBounceVelocity &&
            subCell.mPosition.y - static_cast<float>(row) >= 1.0f) {
            
            animation.mState = FallingBlockAnimationComponent::State::Falling;
            animation.mVelocity = 0.0f;
        }
    }
}

CollapsingFieldAnimationSystem::CollapsingFieldAnimationSystem(Field& field) :
    mField {field} {}

CollapsingFieldAnimationSystem::State CollapsingFieldAnimationSystem::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState(dt);
            break;
        case State::Active:
        case State::BlocksBouncing:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            UpdateInInactiveState();
            break;
    }
    
    return mState;
}

void CollapsingFieldAnimationSystem::UpdateInWaitingState(float dt) {
    mWaitedTime += dt;
    if (mWaitedTime > waitTime) {
        mState = State::Active;
    }
}

void CollapsingFieldAnimationSystem::UpdateInActiveState(float dt) {
    auto anyFallingBlocks = false;
    auto anyBouncingBlocks = false;
    
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            if (cell.IsEmpty()) {
                continue;
            }
            
            switch (UpdateBlock(cell.mFirstSubCell, row, dt)) {
                case FallingBlockAnimationComponent::State::Falling:
                    anyFallingBlocks = true;
                    break;
                case FallingBlockAnimationComponent::State::Bouncing:
                    anyBouncingBlocks = true;
                    break;
                case FallingBlockAnimationComponent::State::Inactive:
                    break;
            }

            switch (UpdateBlock(cell.mSecondSubCell, row, dt)) {
                case FallingBlockAnimationComponent::State::Falling:
                    anyFallingBlocks = true;
                    break;
                case FallingBlockAnimationComponent::State::Bouncing:
                    anyBouncingBlocks = true;
                    break;
                case FallingBlockAnimationComponent::State::Inactive:
                    break;
            }
        }
    }
    
    mField.SetChanged();
    
    if (anyFallingBlocks) {
        mState = State::Active;
    } else if (anyBouncingBlocks) {
        mState = State::BlocksBouncing;
    } else {
        mState = State::Inactive;
    }
}

void CollapsingFieldAnimationSystem::UpdateInInactiveState() {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
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

void CollapsingFieldAnimationSystem::GoToWaitingState() {
    mWaitedTime = 0.0f;
    mState = State::Waiting;
}

void CollapsingFieldAnimationSystem::ResetBlockAnimations() {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            cell.mFirstSubCell.mFallingBlockAnimation = FallingBlockAnimationComponent {};
            cell.mSecondSubCell.mFallingBlockAnimation = FallingBlockAnimationComponent {};
        }
    }
}

void CollapsingFieldAnimationSystem::TransitionWronglyBouncingBlocksToFalling() {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            TransitionWronglyBouncingBlockToFalling(cell.mFirstSubCell, row);
            TransitionWronglyBouncingBlockToFalling(cell.mSecondSubCell, row);
        }
    }
}
