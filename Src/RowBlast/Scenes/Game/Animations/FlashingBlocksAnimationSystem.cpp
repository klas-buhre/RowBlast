#include "FlashingBlocksAnimationSystem.hpp"

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    constexpr auto flashDuration = 0.32f;
}

const Pht::Color FlashingBlocksAnimationSystem::colorAdd {0.32f, 0.32f, 0.32f};
const Pht::Color FlashingBlocksAnimationSystem::semiFlashingColorAdd {0.16f, 0.16f, 0.16f};
const Pht::Color FlashingBlocksAnimationSystem::brightColorAdd {0.53f, 0.53f, 0.53f};

FlashingBlocksAnimationSystem::FlashingBlocksAnimationSystem(Field& field,
                                                             PieceResources& pieceResources) :
    mField {field},
    mPieceResources {pieceResources} {}

void FlashingBlocksAnimationSystem::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
}

void FlashingBlocksAnimationSystem::Start(BlockColor color) {
    mColor = color;
    mState = State::Waiting;
    mElapsedTime = 0.0f;
}

void FlashingBlocksAnimationSystem::Update(float dt) {
    switch (mState) {
        case State::Inactive:
            return;
        case State::Waiting:
            UpdateInWaitingState();
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::OnlyFlyingBlocksFlashing:
            UpdateFlash(dt);
            break;
    }
}

void FlashingBlocksAnimationSystem::UpdateInWaitingState() {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            if (cell.IsEmpty()) {
                continue;
            }
            
            ActivateWaitingBlock(cell.mFirstSubCell, row, column);
            ActivateWaitingBlock(cell.mSecondSubCell, row, column);
        }
    }
    
    if (mState == State::Waiting) {
        mState = State::OnlyFlyingBlocksFlashing;
    }
    
    UpdateRenderables();
}

void FlashingBlocksAnimationSystem::ActivateWaitingBlock(SubCell& subCell, int row, int column) {
    auto& flashingBlockAnimation = subCell.mFlashingBlockAnimation;
    if (flashingBlockAnimation.mState == FlashingBlockAnimationComponent::State::Waiting) {
        if (mState == State::Waiting) {
            mState = State::Active;
        }

        flashingBlockAnimation.mState = FlashingBlockAnimationComponent::State::Active;
        
        if (IsBlockAccordingToBlueprint(subCell, row, column)) {
            flashingBlockAnimation.mBrightness = BlockBrightness::BlueprintFillFlashing;
        } else {
            flashingBlockAnimation.mBrightness = BlockBrightness::Flashing;
        }
    }
}

void FlashingBlocksAnimationSystem::UpdateInActiveState(float dt) {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            if (cell.IsEmpty()) {
                continue;
            }
            
            ActivateWaitingBlock(cell.mFirstSubCell, row, column);
            ActivateWaitingBlock(cell.mSecondSubCell, row, column);
        }
    }
    
    UpdateFlash(dt);
    mField.SetChanged();
}

void FlashingBlocksAnimationSystem::UpdateFlash(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > flashDuration) {
        mState = State::Inactive;
        ResetFlashingBlockAnimations();
    }
    
    UpdateRenderables();
}

void FlashingBlocksAnimationSystem::ResetFlashingBlockAnimations() {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& cell = mField.GetCell(row, column);
            cell.mFirstSubCell.mFlashingBlockAnimation = FlashingBlockAnimationComponent {};
            cell.mSecondSubCell.mFlashingBlockAnimation = FlashingBlockAnimationComponent {};
        }
    }
}

bool FlashingBlocksAnimationSystem::IsBlockAccordingToBlueprint(SubCell& subCell,
                                                                int row,
                                                                int column) {
    auto* blueprintGrid = mField.GetBlueprintGrid();
    if (blueprintGrid == nullptr) {
        return false;
    }
    
    auto blueprintFill = (*blueprintGrid)[row][column].mFill;
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

void FlashingBlocksAnimationSystem::UpdateRenderables() {
    auto flashColorAdd = CalculateFlashColorAdd(colorAdd);
    auto brightFlashColorAdd = CalculateFlashColorAdd(brightColorAdd);
    auto semiFlashColorAdd = CalculateFlashColorAdd(semiFlashingColorAdd);
    
    UpdateBlockRenderables(flashColorAdd, BlockBrightness::Flashing);
    UpdateBlockRenderables(brightFlashColorAdd, BlockBrightness::BlueprintFillFlashing);

    UpdateBondRenderables(flashColorAdd, BlockBrightness::Flashing);
    UpdateBondRenderables(brightFlashColorAdd, BlockBrightness::BlueprintFillFlashing);
    UpdateBondRenderables(semiFlashColorAdd, BlockBrightness::SemiFlashing);
}

void FlashingBlocksAnimationSystem::UpdateBlockRenderables(const Pht::Color& flashColorAdd,
                                                           BlockBrightness flashingBlockBrightness) {
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::Full);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::LowerRightHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::UpperRightHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::UpperLeftHalf);
    UpdateBlockRenderable(flashColorAdd, flashingBlockBrightness, BlockKind::LowerLeftHalf);
}

Pht::Color FlashingBlocksAnimationSystem::CalculateFlashColorAdd(const Pht::Color& flashMaxColorAdd) {
    if (mElapsedTime > flashDuration) {
        return Pht::Color {};
    }
    
    auto factor = (flashDuration - mElapsedTime) / flashDuration;
    
    return {
        flashMaxColorAdd.mRed * factor,
        flashMaxColorAdd.mGreen * factor,
        flashMaxColorAdd.mBlue * factor
    };
}

void FlashingBlocksAnimationSystem::UpdateBlockRenderable(const Pht::Color& flashColorAdd,
                                                          BlockBrightness flashingBlockBrightness,
                                                          BlockKind blockKind) {
    auto& normalRenderable =
        mPieceResources.GetBlockRenderableObject(blockKind, mColor, BlockBrightness::Normal);
    
    auto& flashingRenderable =
        mPieceResources.GetBlockRenderableObject(blockKind, mColor, flashingBlockBrightness);
    
    auto& normalMaterial = normalRenderable.GetMaterial();
    auto& flashingMaterial = flashingRenderable.GetMaterial();
    
    flashingMaterial.SetAmbient(normalMaterial.GetAmbient() + flashColorAdd);
    flashingMaterial.SetDiffuse(normalMaterial.GetDiffuse() + flashColorAdd);
    flashingMaterial.SetSpecular(normalMaterial.GetSpecular() + flashColorAdd);
}

void FlashingBlocksAnimationSystem::UpdateBondRenderables(const Pht::Color& flashColorAdd,
                                                          BlockBrightness flashingBondBrightness) {
    UpdateBondRenderable(flashColorAdd, flashingBondBrightness, BondRenderableKind::Normal);
    UpdateBondRenderable(flashColorAdd, flashingBondBrightness, BondRenderableKind::Aslope);
    UpdateBondRenderable(flashColorAdd, flashingBondBrightness, BondRenderableKind::Diagonal);
}

void FlashingBlocksAnimationSystem::UpdateBondRenderable(const Pht::Color& flashColorAdd,
                                                         BlockBrightness flashingBondBrightness,
                                                         BondRenderableKind bondKind) {
    auto& normalRenderable =
        mPieceResources.GetBondRenderableObject(bondKind, mColor, BlockBrightness::Normal);
    
    auto& flashingRenderable =
        mPieceResources.GetBondRenderableObject(bondKind, mColor, flashingBondBrightness);
    
    auto& normalMaterial = normalRenderable.GetMaterial();
    auto& flashingMaterial = flashingRenderable.GetMaterial();
    
    flashingMaterial.SetAmbient(normalMaterial.GetAmbient() + flashColorAdd);
    flashingMaterial.SetDiffuse(normalMaterial.GetDiffuse() + flashColorAdd);
    flashingMaterial.SetSpecular(normalMaterial.GetSpecular() + flashColorAdd);
}
