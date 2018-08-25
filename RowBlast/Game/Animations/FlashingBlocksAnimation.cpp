#include "FlashingBlocksAnimation.hpp"

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto flashDuration {0.3f};
}

const Pht::Color FlashingBlocksAnimation::colorAdd {0.3f, 0.3f, 0.3f};
const Pht::Color FlashingBlocksAnimation::brightColorAdd {0.53f, 0.53f, 0.53f};
const Pht::Color FlashingBlocksAnimation::semiFlashingColorAdd {0.15f, 0.15f, 0.15f};

FlashingBlocksAnimation::FlashingBlocksAnimation(Field& field, PieceResources& pieceResources) :
    mField {field},
    mPieceResources {pieceResources} {}

void FlashingBlocksAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
}

void FlashingBlocksAnimation::Start(BlockColor color) {
    mColor = color;
    mState = State::Waiting;
    mElapsedTime = 0.0f;
}

void FlashingBlocksAnimation::Update(float dt) {
    switch (mState) {
        case State::Inactive:
            return;
        case State::Waiting:
            UpdateInWaitingState();
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
    }
}

void FlashingBlocksAnimation::UpdateInWaitingState() {
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            ActivateNonBouncingWaitingBlock(cell.mFirstSubCell, row, column);
            ActivateNonBouncingWaitingBlock(cell.mSecondSubCell, row, column);
        }
    }
}

void FlashingBlocksAnimation::ActivateNonBouncingWaitingBlock(SubCell& subCell,
                                                              int row,
                                                              int column) {
    auto& flashingBlockAnimation {subCell.mFlashingBlockAnimation};
    
    if (flashingBlockAnimation.mState == FlashingBlockAnimation::State::Waiting /*&&
        subCell.mFallingBlockAnimation.mState != FallingBlockAnimation::State::Bouncing*/) {
        
        if (mState == State::Waiting) {
            mState = State::Active;
            UpdateRenderables();
        }

        flashingBlockAnimation.mState = FlashingBlockAnimation::State::Active;
        
        if (IsBlockAccordingToBlueprint(subCell, row, column)) {
            flashingBlockAnimation.mBrightness = BlockBrightness::BlueprintFillFlashing;
        } else {
            flashingBlockAnimation.mBrightness = BlockBrightness::Flashing;
        }
    }
}

void FlashingBlocksAnimation::UpdateInActiveState(float dt) {
    for (auto row {0}; row < mField.GetNumRows(); ++row) {
        for (auto column {0}; column < mField.GetNumColumns(); ++column) {
            auto& cell {mField.GetCell(row, column)};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            ActivateNonBouncingWaitingBlock(cell.mFirstSubCell, row, column);
            ActivateNonBouncingWaitingBlock(cell.mSecondSubCell, row, column);
        }
    }
    
    mField.SetChanged();
    mElapsedTime += dt;
    
    if (mElapsedTime > flashDuration) {
        mState = State::Inactive;
        mField.ResetFlashingBlockAnimations();
    }
    
    UpdateRenderables();
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

void FlashingBlocksAnimation::UpdateRenderables() {
    auto flashColorAdd {CalculateFlashColorAdd(colorAdd)};
    auto brightFlashColorAdd {CalculateFlashColorAdd(brightColorAdd)};
    auto semiFlashColorAdd {CalculateFlashColorAdd(semiFlashingColorAdd)};
    
    UpdateBlockRenderables(flashColorAdd, BlockBrightness::Flashing);
    UpdateBlockRenderables(brightFlashColorAdd, BlockBrightness::BlueprintFillFlashing);

    UpdateWeldRenderables(flashColorAdd, BlockBrightness::Flashing);
    UpdateWeldRenderables(brightFlashColorAdd, BlockBrightness::BlueprintFillFlashing);
    UpdateWeldRenderables(semiFlashColorAdd, BlockBrightness::SemiFlashing);
}

void FlashingBlocksAnimation::UpdateBlockRenderables(const Pht::Color& flashColorAdd,
                                                     BlockBrightness flashingBlockBrightness) {
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::Full);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::LowerRightHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::UpperRightHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::UpperLeftHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::LowerLeftHalf);
}

Pht::Color FlashingBlocksAnimation::CalculateFlashColorAdd(const Pht::Color& flashMaxColorAdd) {
    if (mElapsedTime > flashDuration) {
        return Pht::Color {};
    }
    
    auto factor {(flashDuration - mElapsedTime) / flashDuration};
    
    return {
        flashMaxColorAdd.mRed * factor,
        flashMaxColorAdd.mGreen * factor,
        flashMaxColorAdd.mBlue * factor
    };
}

void FlashingBlocksAnimation::UpdateBlockRenderable(const Pht::Color& flashColorAdd,
                                                    BlockBrightness flashingBlockBrightness,
                                                    BlockKind blockKind) {
    auto& normalRenderable {
        mPieceResources.GetBlockRenderableObject(blockKind, mColor, BlockBrightness::Normal)
    };
    
    auto& flashingRenderable {
        mPieceResources.GetBlockRenderableObject(blockKind, mColor, flashingBlockBrightness)
    };
    
    auto& normalMaterial {normalRenderable.GetMaterial()};
    auto& flashingMaterial {flashingRenderable.GetMaterial()};
    
    flashingMaterial.SetAmbient(normalMaterial.GetAmbient() + flashColorAdd);
    flashingMaterial.SetDiffuse(normalMaterial.GetDiffuse() + flashColorAdd);
    flashingMaterial.SetSpecular(normalMaterial.GetSpecular() + flashColorAdd);
}

void FlashingBlocksAnimation::UpdateWeldRenderables(const Pht::Color& flashColorAdd,
                                                    BlockBrightness flashingWeldBrightness) {
    UpdateWeldRenderable(flashColorAdd, flashingWeldBrightness, WeldRenderableKind::Normal);
    UpdateWeldRenderable(flashColorAdd, flashingWeldBrightness, WeldRenderableKind::Aslope);
    UpdateWeldRenderable(flashColorAdd, flashingWeldBrightness, WeldRenderableKind::Diagonal);
}

void FlashingBlocksAnimation::UpdateWeldRenderable(const Pht::Color& flashColorAdd,
                                                   BlockBrightness flashingWeldBrightness,
                                                   WeldRenderableKind weldKind) {
    auto& normalRenderable {
        mPieceResources.GetWeldRenderableObject(weldKind, mColor, BlockBrightness::Normal)
    };
    
    auto& flashingRenderable {
        mPieceResources.GetWeldRenderableObject(weldKind, mColor, flashingWeldBrightness)
    };
    
    auto& normalMaterial {normalRenderable.GetMaterial()};
    auto& flashingMaterial {flashingRenderable.GetMaterial()};
    
    flashingMaterial.SetAmbient(normalMaterial.GetAmbient() + flashColorAdd);
    flashingMaterial.SetDiffuse(normalMaterial.GetDiffuse() + flashColorAdd);
    flashingMaterial.SetSpecular(normalMaterial.GetSpecular() + flashColorAdd);
}
