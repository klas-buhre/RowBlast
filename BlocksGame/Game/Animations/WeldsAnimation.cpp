#include "WeldsAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto scaleSpeed {3.2f};
}

WeldsAnimation::WeldsAnimation(Field& field) :
    mField {field} {}

void WeldsAnimation::Update(float dt) {
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            Pht::IVec2 position {column, row};
            
            AnimateBlockWelds(cell.mFirstSubCell, position, dt);
            AnimateBlockWelds(cell.mSecondSubCell, position, dt);
            
            auto& diagonalAnimation {cell.mSecondSubCell.mWelds.mAnimations.mDiagonal};
            
            if (diagonalAnimation.IsActive()) {
                if (diagonalAnimation.mState == WeldAnimation::State::WeldAtFullScale) {
                    diagonalAnimation.mState = WeldAnimation::State::Inactive;
                    mField.MergeTriangleBlocksIntoCube(position);
                    mField.SetChanged();
                } else {
                    auto cellIsFlashing {
                        cell.mFirstSubCell.mFlashingBlockAnimation.mIsActive ||
                        cell.mSecondSubCell.mFlashingBlockAnimation.mIsActive
                    };
                    
                    AnimateWeld(diagonalAnimation, cellIsFlashing, dt);
                }
            }
        }
    }
}

void WeldsAnimation::AnimateBlockWelds(SubCell& subCell, const Pht::IVec2& position, float dt) {
    auto& animations {subCell.mWelds.mAnimations};
    
    if (animations.mUp.IsActive()) {
        AnimateUpWeld(subCell, position, dt);
    }
    
    if (animations.mRight.IsActive()) {
        AnimateRightWeld(subCell, position, dt);
    }
    
    AnimateWeld(animations.mUpRight, false, dt);
    AnimateWeld(animations.mUpLeft, false, dt);
}

void WeldsAnimation::AnimateUpWeld(SubCell& subCell, const Pht::IVec2& position, float dt) {
    auto subCellIsFlashing {subCell.mFlashingBlockAnimation.mIsActive};
    
    if (position.y + 1 < mField.GetNumRows()) {
        auto& upperCell {mField.GetCell(position + Pht::IVec2{0, 1})};

        auto upperCellIsFlashing {
            upperCell.mFirstSubCell.mFlashingBlockAnimation.mIsActive ||
            upperCell.mSecondSubCell.mFlashingBlockAnimation.mIsActive
        };
        
        AnimateWeld(subCell.mWelds.mAnimations.mUp, subCellIsFlashing || upperCellIsFlashing, dt);
    } else {
        AnimateWeld(subCell.mWelds.mAnimations.mUp, subCellIsFlashing, dt);
    }
}

void WeldsAnimation::AnimateRightWeld(SubCell& subCell, const Pht::IVec2& position, float dt) {
    auto subCellIsFlashing {subCell.mFlashingBlockAnimation.mIsActive};
    
    if (position.x + 1 < mField.GetNumColumns()) {
        auto& cellToTheRight {mField.GetCell(position + Pht::IVec2{1, 0})};
        
        auto cellToTheRightIsFlashing {
            cellToTheRight.mFirstSubCell.mFlashingBlockAnimation.mIsActive ||
            cellToTheRight.mSecondSubCell.mFlashingBlockAnimation.mIsActive
        };
        
        AnimateWeld(subCell.mWelds.mAnimations.mRight, subCellIsFlashing || cellToTheRightIsFlashing, dt);
    } else {
        AnimateWeld(subCell.mWelds.mAnimations.mRight, subCellIsFlashing, dt);
    }
}

void WeldsAnimation::AnimateWeldAppearing(WeldAnimation& animation, float dt) {
    animation.mScale += scaleSpeed * dt;
    
    if (animation.mScale > 1.0f) {
        animation.mScale = 1.0f;
        animation.mState = WeldAnimation::State::WeldAtFullScale;
    }

    mField.SetChanged();
}

void WeldsAnimation::AnimateWeldDisappearing(WeldAnimation& animation, float dt) {
    animation.mScale -= scaleSpeed * dt;
    
    if (animation.mScale < 0.0f) {
        animation.mScale = 0.0f;
        animation.mState = WeldAnimation::State::Inactive;
    }

    mField.SetChanged();
}

void WeldsAnimation::AnimateWeld(WeldAnimation& animation, bool cellIsFlashing, float dt) {
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

void WeldsAnimation::StartWeldAppearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldAppearing;
    animation.mScale = 0.0f;
}

void WeldsAnimation::StartWeldDisappearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldDisappearing;
    animation.mScale = 1.0f;
}
