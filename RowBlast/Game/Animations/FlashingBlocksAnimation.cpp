#include "FlashingBlocksAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto flashDuration {0.22f};
    
    void UpdateBlockInWaitingState(SubCell& subCell, float dt) {
        if (subCell.mFallingBlockAnimation.mState != FallingBlockAnimation::State::Bouncing) {
            subCell.mFlashingBlockAnimation.mState = FlashingBlockAnimation::State::Active;
        }
    }
}

const Pht::Color FlashingBlocksAnimation::colorAdd {0.23f, 0.23f, 0.23f};
const Pht::Color FlashingBlocksAnimation::brightColorAdd {0.53f, 0.53f, 0.53f};
const Pht::Color FlashingBlocksAnimation::semiFlashingColorAdd {0.115f, 0.115f, 0.115f};

FlashingBlocksAnimation::FlashingBlocksAnimation(Field& field) :
    mField {field} {}

void FlashingBlocksAnimation::Update(float dt) {
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            AnimateFlashingBlock(cell.mFirstSubCell, row, column, dt);
            AnimateFlashingBlock(cell.mSecondSubCell, row, column, dt);
        }
    }
}

void FlashingBlocksAnimation::AnimateFlashingBlock(SubCell& subCell,
                                                   int row,
                                                   int column,
                                                   float dt) {
    switch (subCell.mFlashingBlockAnimation.mState) {
        case FlashingBlockAnimation::State::Inactive:
            return;
        case FlashingBlockAnimation::State::Waiting:
            UpdateBlockInWaitingState(subCell, dt);
            break;
        case FlashingBlockAnimation::State::Active:
            UpdateBlockInActiveState(subCell, row, column, dt);
            break;
    }
}

void FlashingBlocksAnimation::UpdateBlockInActiveState(SubCell& subCell,
                                                       int row,
                                                       int column,
                                                       float dt) {
    auto& flashingBlockAnimation {subCell.mFlashingBlockAnimation};
    flashingBlockAnimation.mElapsedTime += dt;
    
    mField.SetChanged();
    
    if (IsBlockAccordingToBlueprint(subCell, row, column)) {
        flashingBlockAnimation.mBrightness = BlockBrightness::BlueprintFillFlashing;
    } else {
        flashingBlockAnimation.mBrightness = BlockBrightness::Flashing;
    }
    
    if (flashingBlockAnimation.mElapsedTime >= flashDuration) {
        flashingBlockAnimation = FlashingBlockAnimation {};
    }
}

bool FlashingBlocksAnimation::IsBlockAccordingToBlueprint(SubCell& subCell, int row, int column) {
    auto* blueprintGrid {mField.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return false;
    }
    
    auto blueprintFill {(*blueprintGrid)[row][column].mFill};
    
    switch (blueprintFill) {
        case Fill::Full:
            return true;
        case Fill::Empty:
            return false;
        default:
            if (blueprintFill == subCell.mFill) {
                return true;
            }
            return false;
    }
}
