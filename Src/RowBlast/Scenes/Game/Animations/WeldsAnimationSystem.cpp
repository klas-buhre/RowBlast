#include "WeldsAnimationSystem.hpp"

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto scaleSpeed = 3.2f;
    
    bool IsSubCellOrNeighbourBouncing(const SubCell& subCell, const Cell& neighbour) {
        auto subCellIsBouncing =
            subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing;

        auto neighbourIsBouncing =
            neighbour.mFirstSubCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing ||
            neighbour.mSecondSubCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing;

        return subCellIsBouncing || neighbourIsBouncing;
    }
    
    bool IsSubCellBouncing(const SubCell& subCell) {
        return subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing;
    }
}

WeldsAnimationSystem::WeldsAnimationSystem(Field& field) :
    mField {field} {}

void WeldsAnimationSystem::Update(float dt) {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            if (cell.IsEmpty()) {
                continue;
            }
            
            Pht::IVec2 position {column, row};
            
            AnimateBlockWelds(cell.mFirstSubCell, position, dt);
            AnimateBlockWelds(cell.mSecondSubCell, position, dt);
            
            auto& diagonalAnimation = cell.mSecondSubCell.mWeldAnimations.mDiagonal;
            if (diagonalAnimation.IsActive() && !IsSubCellBouncing(cell.mFirstSubCell) &&
                !IsSubCellBouncing(cell.mSecondSubCell)) {
                
                if (diagonalAnimation.mState == WeldAnimation::State::WeldAtFullScale) {
                    diagonalAnimation.mState = WeldAnimation::State::Inactive;
                    mField.MergeTriangleBlocksIntoCube(position);
                    mField.SetChanged();
                } else {
                    auto cellIsFlashing =
                        cell.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
                        cell.mSecondSubCell.mFlashingBlockAnimation.IsActive();
                    
                    AnimateWeld(diagonalAnimation, cellIsFlashing, false, dt);
                }
            }
        }
    }
}

void WeldsAnimationSystem::AnimateBlockWelds(SubCell& subCell,
                                             const Pht::IVec2& position,
                                             float dt) {
    auto& animations = subCell.mWeldAnimations;
    if (animations.mUp.IsActive()) {
        AnimateUpWeld(subCell, position, dt);
    }
    
    if (animations.mRight.IsActive()) {
        AnimateRightWeld(subCell, position, dt);
    }
    
    auto isThisOrUpRightBouncing = false;
    auto isThisOrUpLeftBouncing = false;
    
    if (position.y + 1 < mField.GetNumRows() && position.x + 1 < mField.GetNumColumns()) {
        auto& upperRightCell = mField.GetCell(position + Pht::IVec2{1, 1});
        isThisOrUpRightBouncing = IsSubCellOrNeighbourBouncing(subCell, upperRightCell);
    } else {
        isThisOrUpRightBouncing = IsSubCellBouncing(subCell);
    }

    if (position.y + 1 < mField.GetNumRows() && position.x - 1 >= 0) {
        auto& upperLeftCell = mField.GetCell(position + Pht::IVec2{-1, 1});
        isThisOrUpLeftBouncing = IsSubCellOrNeighbourBouncing(subCell, upperLeftCell);
    } else {
        isThisOrUpLeftBouncing = IsSubCellBouncing(subCell);
    }
    
    AnimateWeld(animations.mUpRight, false, isThisOrUpRightBouncing, dt);
    AnimateWeld(animations.mUpLeft, false, isThisOrUpLeftBouncing, dt);
}

void WeldsAnimationSystem::AnimateUpWeld(SubCell& subCell, const Pht::IVec2& position, float dt) {
    auto subCellIsFlashing = subCell.mFlashingBlockAnimation.IsActive();
    
    if (position.y + 1 < mField.GetNumRows()) {
        auto& upperCell {mField.GetCell(position + Pht::IVec2{0, 1})};

        auto upperCellIsFlashing =
            upperCell.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
            upperCell.mSecondSubCell.mFlashingBlockAnimation.IsActive();
        
        AnimateWeld(subCell.mWeldAnimations.mUp,
                    subCellIsFlashing || upperCellIsFlashing,
                    IsSubCellOrNeighbourBouncing(subCell, upperCell),
                    dt);
    } else {
        AnimateWeld(subCell.mWeldAnimations.mUp, subCellIsFlashing, IsSubCellBouncing(subCell), dt);
    }
}

void WeldsAnimationSystem::AnimateRightWeld(SubCell& subCell,
                                            const Pht::IVec2& position,
                                            float dt) {
    auto subCellIsFlashing = subCell.mFlashingBlockAnimation.IsActive();
    
    if (position.x + 1 < mField.GetNumColumns()) {
        auto& cellToTheRight = mField.GetCell(position + Pht::IVec2{1, 0});
        
        auto cellToTheRightIsFlashing =
            cellToTheRight.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
            cellToTheRight.mSecondSubCell.mFlashingBlockAnimation.IsActive();
        
        AnimateWeld(subCell.mWeldAnimations.mRight,
                    subCellIsFlashing || cellToTheRightIsFlashing,
                    IsSubCellOrNeighbourBouncing(subCell, cellToTheRight),
                    dt);
    } else {
        AnimateWeld(subCell.mWeldAnimations.mRight,
                    subCellIsFlashing,
                    IsSubCellBouncing(subCell),
                    dt);
    }
}

void WeldsAnimationSystem::AnimateWeldAppearing(WeldAnimation& animation, float dt) {
    animation.mScale += scaleSpeed * dt;
    if (animation.mScale > 1.0f) {
        animation.mScale = 1.0f;
        animation.mState = WeldAnimation::State::WeldAtFullScale;
    }

    mField.SetChanged();
}

void WeldsAnimationSystem::AnimateWeldDisappearing(WeldAnimation& animation, float dt) {
    animation.mScale -= scaleSpeed * dt;
    if (animation.mScale < 0.0f) {
        animation.mScale = 0.0f;
        animation.mState = WeldAnimation::State::Inactive;
    }

    mField.SetChanged();
}

void WeldsAnimationSystem::AnimateWeld(WeldAnimation& animation,
                                       bool cellIsFlashing,
                                       bool anyBouncing,
                                       float dt) {
    if (anyBouncing) {
        return;
    }
    
    switch (animation.mState) {
        case WeldAnimation::State::WeldAppearing:
            if (cellIsFlashing) {
                animation.mState = WeldAnimation::State::WeldAppearingAndSemiFlashing;
            }
            AnimateWeldAppearing(animation, dt);
            break;
        case WeldAnimation::State::WeldAppearingAndSemiFlashing:
            if (!cellIsFlashing) {
                animation.mState = WeldAnimation::State::WeldAppearing;
            }
            AnimateWeldAppearing(animation, dt);
            break;
        case WeldAnimation::State::WeldAtFullScale:
            animation.mState = WeldAnimation::State::Inactive;
            break;
        case WeldAnimation::State::WeldDisappearing:
            AnimateWeldDisappearing(animation, dt);
            break;
        case WeldAnimation::State::Inactive:
            break;
    }
}

void WeldsAnimationSystem::StartWeldAppearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldAppearing;
    animation.mScale = 0.0f;
}

void WeldsAnimationSystem::StartWeldDisappearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldDisappearing;
    animation.mScale = 1.0f;
}
