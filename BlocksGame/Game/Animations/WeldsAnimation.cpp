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
            
            AnimateBlockWelds(cell.mFirstSubCell, dt);
            AnimateBlockWelds(cell.mSecondSubCell, dt);
        }
    }
}

void WeldsAnimation::AnimateBlockWelds(SubCell& subCell, float dt) {
    auto& animations {subCell.mWelds.mAnimations};
    
    AnimateWeld(animations.mUp, dt);
    AnimateWeld(animations.mRight, dt);
    AnimateWeld(animations.mUpRight, dt);
    AnimateWeld(animations.mUpLeft, dt);
    AnimateWeld(animations.mDiagonal, dt);
}

void WeldsAnimation::AnimateWeld(WeldAnimation& animation, float dt) {
    switch (animation.mState) {
        case WeldAnimation::State::Inactive:
            return;
        case WeldAnimation::State::WeldAppearing:
            AnimateWeldAppearing(animation, dt);
            break;
        case WeldAnimation::State::WeldDisappearing:
            AnimateWeldDisappearing(animation, dt);
            break;
    }
}

void WeldsAnimation::AnimateWeldAppearing(WeldAnimation& animation, float dt) {
    animation.mScale += scaleSpeed * dt;
    
    if (animation.mScale > 1.0f) {
        animation.mScale = 1.0f;
        animation.mState = WeldAnimation::State::Inactive;
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

void WeldsAnimation::StartWeldAppearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldAppearing;
    animation.mScale = 0.0f;
}

void WeldsAnimation::StartWeldDisappearingAnimation(WeldAnimation& animation) {
    animation.mState = WeldAnimation::State::WeldDisappearing;
    animation.mScale = 1.0f;
}
