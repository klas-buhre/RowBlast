#include "BlueprintSlotsFilledAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace BlocksGame;

namespace {
    const auto duration {0.35f};
    const auto scaleUpSpeed {3.2f};
    const auto fadeSpeed {BlueprintSlotFillAnimation::mInitialOpacity / duration};
    
    void AnimateSlot(BlueprintSlotFillAnimation& animation, float dt) {
        animation.mElapsedTime += dt;
        
        if (animation.mElapsedTime > duration) {
            animation = BlueprintSlotFillAnimation {};
            return;
        }
        
        animation.mScale += scaleUpSpeed * dt;
        animation.mOpacity -= fadeSpeed * dt;
        
        if (animation.mOpacity < 0.0f) {
            animation = BlueprintSlotFillAnimation {};
        }
    }
}

BlueprintSlotsFilledAnimation::BlueprintSlotsFilledAnimation(Field& field) :
    mField {field} {}

void BlueprintSlotsFilledAnimation::Update(float dt) {
    auto* blueprintGrid {mField.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return;
    }
    
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& slotFilledAnimation {(*blueprintGrid)[row][column].mAnimation};
            
            if (slotFilledAnimation.mIsActive) {
                AnimateSlot(slotFilledAnimation, dt);
            }
        }
    }
}
