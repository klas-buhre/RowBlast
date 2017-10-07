#include "CollapsingFieldAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace BlocksGame;

namespace {
    const auto gravitationalAcceleration {-35.0f};
    const auto waitTime {0.15f};
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
        GoToActiveState();
    }
}

void CollapsingFieldAnimation::GoToActiveState() {
    mVelocity = 0.0f;
    mState = State::Active;
}

void CollapsingFieldAnimation::UpdateInActiveState(float dt) {
    mVelocity += gravitationalAcceleration * dt;
    auto velocity {mVelocity * dt};
    auto anyMovingCells {false};
    
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            auto& firstSubCellPosition {cell.mFirstSubCell.mPosition};
            
            if (firstSubCellPosition.y > row) {
                firstSubCellPosition.y += velocity;
                
                if (firstSubCellPosition.y < row) {
                    firstSubCellPosition.y = row;
                } else {
                    anyMovingCells = true;
                }
            }
            
            auto& secondSubCellPosition {cell.mSecondSubCell.mPosition};
            
            if (secondSubCellPosition.y > row) {
                secondSubCellPosition.y += velocity;
                
                if (secondSubCellPosition.y < row) {
                    secondSubCellPosition.y = row;
                } else {
                    anyMovingCells = true;
                }
            }
        }
    }
    
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
