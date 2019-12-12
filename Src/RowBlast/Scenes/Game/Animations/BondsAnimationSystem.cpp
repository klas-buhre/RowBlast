#include "BondsAnimationSystem.hpp"

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

BondsAnimationSystem::BondsAnimationSystem(Field& field) :
    mField {field} {}

void BondsAnimationSystem::Update(float dt) {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            if (cell.IsEmpty()) {
                continue;
            }
            
            Pht::IVec2 position {column, row};
            
            AnimateBlockBonds(cell.mFirstSubCell, position, dt);
            AnimateBlockBonds(cell.mSecondSubCell, position, dt);
            
            auto& diagonalAnimation = cell.mSecondSubCell.mBondAnimations.mDiagonal;
            if (diagonalAnimation.IsActive() && !IsSubCellBouncing(cell.mFirstSubCell) &&
                !IsSubCellBouncing(cell.mSecondSubCell)) {
                
                if (diagonalAnimation.mState == BondAnimation::State::BondAtFullScale) {
                    diagonalAnimation.mState = BondAnimation::State::Inactive;
                    mField.MergeTriangleBlocksIntoCube(position);
                    mField.SetChanged();
                } else {
                    auto cellIsFlashing =
                        cell.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
                        cell.mSecondSubCell.mFlashingBlockAnimation.IsActive();
                    
                    AnimateBond(diagonalAnimation, cellIsFlashing, false, dt);
                }
            }
        }
    }
}

void BondsAnimationSystem::AnimateBlockBonds(SubCell& subCell,
                                             const Pht::IVec2& position,
                                             float dt) {
    auto& animations = subCell.mBondAnimations;
    if (animations.mUp.IsActive()) {
        AnimateUpBond(subCell, position, dt);
    }
    
    if (animations.mRight.IsActive()) {
        AnimateRightBond(subCell, position, dt);
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
    
    AnimateBond(animations.mUpRight, false, isThisOrUpRightBouncing, dt);
    AnimateBond(animations.mUpLeft, false, isThisOrUpLeftBouncing, dt);
}

void BondsAnimationSystem::AnimateUpBond(SubCell& subCell, const Pht::IVec2& position, float dt) {
    auto subCellIsFlashing = subCell.mFlashingBlockAnimation.IsActive();
    
    if (position.y + 1 < mField.GetNumRows()) {
        auto& upperCell {mField.GetCell(position + Pht::IVec2{0, 1})};

        auto upperCellIsFlashing =
            upperCell.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
            upperCell.mSecondSubCell.mFlashingBlockAnimation.IsActive();
        
        AnimateBond(subCell.mBondAnimations.mUp,
                    subCellIsFlashing || upperCellIsFlashing,
                    IsSubCellOrNeighbourBouncing(subCell, upperCell),
                    dt);
    } else {
        AnimateBond(subCell.mBondAnimations.mUp, subCellIsFlashing, IsSubCellBouncing(subCell), dt);
    }
}

void BondsAnimationSystem::AnimateRightBond(SubCell& subCell,
                                            const Pht::IVec2& position,
                                            float dt) {
    auto subCellIsFlashing = subCell.mFlashingBlockAnimation.IsActive();
    
    if (position.x + 1 < mField.GetNumColumns()) {
        auto& cellToTheRight = mField.GetCell(position + Pht::IVec2{1, 0});
        
        auto cellToTheRightIsFlashing =
            cellToTheRight.mFirstSubCell.mFlashingBlockAnimation.IsActive() ||
            cellToTheRight.mSecondSubCell.mFlashingBlockAnimation.IsActive();
        
        AnimateBond(subCell.mBondAnimations.mRight,
                    subCellIsFlashing || cellToTheRightIsFlashing,
                    IsSubCellOrNeighbourBouncing(subCell, cellToTheRight),
                    dt);
    } else {
        AnimateBond(subCell.mBondAnimations.mRight,
                    subCellIsFlashing,
                    IsSubCellBouncing(subCell),
                    dt);
    }
}

void BondsAnimationSystem::AnimateBondAppearing(BondAnimation& animation, float dt) {
    animation.mScale += scaleSpeed * dt;
    if (animation.mScale > 1.0f) {
        animation.mScale = 1.0f;
        animation.mState = BondAnimation::State::BondAtFullScale;
    }

    mField.SetChanged();
}

void BondsAnimationSystem::AnimateBondDisappearing(BondAnimation& animation, float dt) {
    animation.mScale -= scaleSpeed * dt;
    if (animation.mScale < 0.0f) {
        animation.mScale = 0.0f;
        animation.mState = BondAnimation::State::Inactive;
    }

    mField.SetChanged();
}

void BondsAnimationSystem::AnimateBond(BondAnimation& animation,
                                       bool cellIsFlashing,
                                       bool anyBouncing,
                                       float dt) {
    if (anyBouncing) {
        return;
    }
    
    switch (animation.mState) {
        case BondAnimation::State::BondAppearing:
            if (cellIsFlashing) {
                animation.mState = BondAnimation::State::BondAppearingAndSemiFlashing;
            }
            AnimateBondAppearing(animation, dt);
            break;
        case BondAnimation::State::BondAppearingAndSemiFlashing:
            if (!cellIsFlashing) {
                animation.mState = BondAnimation::State::BondAppearing;
            }
            AnimateBondAppearing(animation, dt);
            break;
        case BondAnimation::State::BondAtFullScale:
            animation.mState = BondAnimation::State::Inactive;
            break;
        case BondAnimation::State::BondDisappearing:
            AnimateBondDisappearing(animation, dt);
            break;
        case BondAnimation::State::Inactive:
            break;
    }
}

void BondsAnimationSystem::StartBondAppearingAnimation(BondAnimation& animation) {
    animation.mState = BondAnimation::State::BondAppearing;
    animation.mScale = 0.0f;
}

void BondsAnimationSystem::StartBondDisappearingAnimation(BondAnimation& animation) {
    animation.mState = BondAnimation::State::BondDisappearing;
    animation.mScale = 1.0f;
}
