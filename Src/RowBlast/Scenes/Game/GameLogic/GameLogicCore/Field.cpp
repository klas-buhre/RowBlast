#include "Field.hpp"

#include <assert.h>

// Game includes.
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "Level.hpp"
#include "CollisionDetection.hpp"
#include "BondsAnimationSystem.hpp"

using namespace RowBlast;

namespace {
    constexpr auto maxNumScanTries = Field::maxNumRows;
    constexpr auto maxNumRowsInOneScreen = 18;
    const SubCell fullSubCell {Fill::Full};

    bool RowIsFull(const std::vector<Cell>& row) {
        for (auto& cell: row) {
            if (!cell.IsFull() || cell.mFirstSubCell.IsNonBlockObject()) {
                return false;
            }
        }

        return true;
    }
    
    float CalculatePieceCellContribution(const SubCell& subCell, int pieceId) {
        if (subCell.mPieceId != pieceId) {
            return 0.0f;
        }
        
        switch (subCell.mFill) {
            case Fill::Empty:
                return 0.0f;
            case Fill::LowerRightHalf:
            case Fill::UpperRightHalf:
            case Fill::UpperLeftHalf:
            case Fill::LowerLeftHalf:
                return 0.5f;
            case Fill::Full:
                return 1.0f;
        }
    }

    float CalculateGrayLevelCellContribution(const SubCell& subCell) {
        if (!subCell.mIsGrayLevelBlock) {
            return 0.0f;
        }
        
        switch (subCell.mFill) {
            case Fill::Empty:
                return 0.0f;
            case Fill::LowerRightHalf:
            case Fill::UpperRightHalf:
            case Fill::UpperLeftHalf:
            case Fill::LowerLeftHalf:
                return 0.5f;
            case Fill::Full:
                return 1.0f;
        }
    }

    void BreakDownBonds(SubCell& subCell) {
        auto& bonds = subCell.mBonds;
        bonds.mDownRight = false;
        bonds.mDown = false;
        bonds.mDownLeft = false;
    }
    
    void BreakUpBonds(SubCell& subCell) {
        auto& bonds = subCell.mBonds;
        bonds.mUpRight = false;
        bonds.mUp = false;
        bonds.mUpLeft = false;
        
        auto& animations = subCell.mBondAnimations;
        animations.mUp = BondAnimation {};
        animations.mUpRight = BondAnimation {};
        animations.mUpLeft = BondAnimation {};
    }
    
    void BreakRightBonds(SubCell& subCell) {
        auto& bonds = subCell.mBonds;
        bonds.mUpRight = false;
        bonds.mRight = false;
        bonds.mDownRight = false;
        
        auto& animations = subCell.mBondAnimations;
        animations.mUpRight = BondAnimation {};
        animations.mRight = BondAnimation {};
    }
    
    void BreakLeftBonds(SubCell& subCell) {
        auto& bonds = subCell.mBonds;
        bonds.mDownLeft = false;
        bonds.mLeft = false;
        bonds.mUpLeft = false;
    }
    
    PieceBlocks CreatePieceBlocks(const FallingPiece& fallingPiece) {
        auto& pieceType = fallingPiece.GetPieceType();
    
        return {
            pieceType.GetGrid(fallingPiece.GetRotation()),
            pieceType.GetGridNumRows(),
            pieceType.GetGridNumColumns()
        };
    }
}

void Field::Init(const Level& level) {
    mBlueprintGrid = nullptr;
    mNumColumns = level.GetNumColumns();
    mNumRows = level.GetNumRows();
    
    switch (level.GetObjective()) {
        case Level::Objective::Clear: {
            auto* clearGrid = level.GetClearGrid();
            assert(clearGrid);
            mGrid = *clearGrid;
            assert(mNumRows - CalculateHighestLevelBlock().GetValue() >= 14);
            break;
        }
        case Level::Objective::BringDownTheAsteroid: {
            auto* clearGrid = level.GetClearGrid();
            assert(clearGrid);
            mGrid = *clearGrid;
            assert(mNumRows - CalculateAsteroidRow().GetValue() >= 13);
            break;
        }
        case Level::Objective::Build: {
            auto* blueprintGrid = level.GetBlueprintGrid();
            assert(blueprintGrid);
            mBlueprintGrid = std::make_unique<BlueprintCellGrid>(*blueprintGrid);
            mGrid.clear();
            mGrid.reserve(mNumRows);
            std::vector<Cell> emptyRow(mNumColumns);
            for (auto rowIndex = 0; rowIndex < mNumRows; ++rowIndex) {
                mGrid.push_back(emptyRow);
            }
            break;
        }
    }
    
    mLowestVisibleRow = 0;
    ManageBonds();

    mPreviousGrid = mGrid;
    mTempGrid = mGrid;
    SetChanged();
}

void Field::OnEndOfFrame() {
    mHasChanged = false;
}

void Field::SetChanged() {
    mHasChanged = true;
}

void Field::RestorePreviousState() {
    SetChanged();
    CopyGridNoAlloc(mGrid, mPreviousGrid);
}

void Field::SaveState() {
    CopyGridNoAlloc(mPreviousGrid, mGrid);
}

void Field::SaveInTempGrid() {
    CopyGridNoAlloc(mTempGrid, mGrid);
}

void Field::RestoreFromTempGrid() {
    SetChanged();
    CopyGridNoAlloc(mGrid, mTempGrid);
}

void Field::CopyGridNoAlloc(CellGrid& to, const CellGrid& from) {
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            to[row][column] = from[row][column];
        }
    }
}

int Field::GetNumRowsInOneScreen() const {
    if (mNumRows > maxNumRowsInOneScreen) {
        return maxNumRowsInOneScreen;
    }
    
    return mNumRows;
}

bool Field::AnyFilledRows() const {
    auto pastHighestVisibleRow = mLowestVisibleRow + GetNumRowsInOneScreen();
    
    for (auto rowIndex = mLowestVisibleRow; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        if (RowIsFull(mGrid[rowIndex])) {
            return true;
        }
    }
    
    return false;
}

int Field::CalculateNumLevelBlocks() const {
    auto result = 0;
    
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            if (cell.mFirstSubCell.mIsGrayLevelBlock || cell.mSecondSubCell.mIsGrayLevelBlock) {
                ++result;
            }
        }
    }
    
    return result;
}

int Field::CalculateNumEmptyBlueprintSlots() const {
    assert(mBlueprintGrid);
    auto result = 0;
    
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            if ((*mBlueprintGrid)[row][column].mFill != Fill::Empty &&
                !IsCellAccordingToBlueprint(row, column)) {
                ++result;
            }
        }
    }

    return result;
}

Pht::Optional<int> Field::CalculateHighestLevelBlock() const {
    for (auto row = mNumRows - 1; row >= 0; --row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            if (mGrid[row][column].mFirstSubCell.mIsGrayLevelBlock) {
                return row;
            }
        }
    }

    return Pht::Optional<int> {};
}

Pht::Optional<int> Field::CalculateHighestBlockInSpawningArea(int lowestVisibleRow) const {
    auto spawningAreaBottomRow = lowestVisibleRow + numRowsUpToSpawningArea;
    auto spawningAreaTopRow = spawningAreaBottomRow + Piece::maxRows - 1;
    
    for (auto row = spawningAreaTopRow; row >= spawningAreaBottomRow; --row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            if (row <= mNumRows - 1 && !mGrid[row][column].IsEmpty()) {
                return row;
            }
        }
    }

    return Pht::Optional<int> {};
}

Pht::Optional<int> Field::CalculateAsteroidRow() const {
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            if (mGrid[row][column].mFirstSubCell.IsAsteroid()) {
                return row;
            }
        }
    }
    
    return Pht::Optional<int> {};
}

int Field::AccordingToBlueprintHeight() const {
    assert(mBlueprintGrid);
    
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            if (!IsCellAccordingToBlueprint(row, column)) {
                return row;
            }
        }
    }

    return mNumRows;
}

bool Field::IsCellAccordingToBlueprint(int row, int column) const {
    auto& cell = mGrid[row][column];
    auto blueprintFill = (*mBlueprintGrid)[row][column].mFill;
    
    if (cell.mFirstSubCell.IsNonBlockObject()) {
        return false;
    }
    
    switch (blueprintFill) {
        case Fill::Full:
            if (!cell.IsFull()) {
                return false;
            }
            break;
        case Fill::Empty:
            break;
        default:
            if (!cell.IsFull()) {
                if (cell.mFirstSubCell.mFill != blueprintFill &&
                    cell.mSecondSubCell.mFill != blueprintFill) {
                    return false;
                }
            }
            break;
    }
    
    return true;
}

int Field::DetectCollisionDown(const PieceBlocks& pieceBlocks,
                               const Pht::IVec2& position,
                               ValidArea* validArea) const {
    Pht::IVec2 step {0, -1};
    auto collisionPosition = ScanUntilCollision(pieceBlocks, position, step, validArea);
    return collisionPosition.y + 1;
}

int Field::DetectCollisionRight(const PieceBlocks& pieceBlocks,
                                const Pht::IVec2& position,
                                ValidArea* validArea) const {
    Pht::IVec2 step {1, 0};
    auto collisionPosition = ScanUntilCollision(pieceBlocks, position, step, validArea);
    return collisionPosition.x - 1;
}

int Field::DetectCollisionLeft(const PieceBlocks& pieceBlocks,
                               const Pht::IVec2& position,
                               ValidArea* validArea) const {
    Pht::IVec2 step {-1, 0};
    auto collisionPosition = ScanUntilCollision(pieceBlocks, position, step, validArea);
    return collisionPosition.x + 1;
}

Pht::IVec2 Field::ScanUntilCollision(const PieceBlocks& pieceBlocks,
                                     Pht::IVec2 position,
                                     const Pht::IVec2& step,
                                     ValidArea* validArea) const {
    auto isScanStart = true;
    
    for (;;) {
        CheckCollision(mCollisionResult, pieceBlocks, position, step, isScanStart, validArea);
        
        if (isScanStart) {
            isScanStart = false;
        }
        
        if (mCollisionResult.mIsCollision == IsCollision::Yes) {
            return position;
        }
        
        if (mCollisionResult.mIsCollision == IsCollision::NextWillBe) {
            position += step;
            return position;
        }
        
        position += step;
    }

    return position;
}

void Field::CheckCollision(CollisionResult& result,
                           const PieceBlocks& pieceBlocks,
                           const Pht::IVec2& position,
                           const Pht::IVec2& scanDirection,
                           bool isScanStart,
                           ValidArea* validArea) const {
    result.mIsCollision = IsCollision::No;
    result.mCollisionPoints.Clear();
    
    auto pieceNumRows = pieceBlocks.mNumRows;
    auto pieceNumColumns = pieceBlocks.mNumColumns;
    auto& pieceGrid = pieceBlocks.mGrid;

    for (auto pieceRow = 0; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto& pieceCell = pieceGrid[pieceRow][pieceColumn];
            
            auto& pieceSubCell =
                pieceCell.mSecondSubCell.IsEmpty() ? pieceCell.mFirstSubCell : fullSubCell;
            
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto fieldRow = position.y + pieceRow;
            auto fieldColumn = position.x + pieceColumn;
            if (fieldRow < mLowestVisibleRow || fieldRow >= mNumRows || fieldColumn < 0 ||
                fieldColumn >= mNumColumns) {

                result.mCollisionPoints.PushBack(Pht::IVec2{pieceColumn, pieceRow});
                continue;
            }

            auto& fieldCell = mGrid[fieldRow][fieldColumn];
            
            auto firstSubCellIntersects =
                CollisionDetection::SubCellsIntersect(fieldCell.mFirstSubCell,
                                                      pieceSubCell,
                                                      scanDirection,
                                                      isScanStart);
            
            auto secondSubCellIntersects =
                CollisionDetection::SubCellsIntersect(fieldCell.mSecondSubCell,
                                                      pieceSubCell,
                                                      scanDirection,
                                                      isScanStart);
            
            if (firstSubCellIntersects == Intersection::Yes ||
                secondSubCellIntersects == Intersection::Yes) {
                result.mCollisionPoints.PushBack(Pht::IVec2{pieceColumn, pieceRow});
            }
            
            if (firstSubCellIntersects == Intersection::NextWillBe ||
                secondSubCellIntersects == Intersection::NextWillBe) {
                result.mIsCollision = IsCollision::NextWillBe;
            }
        }
    }
    
    if (result.mCollisionPoints.Size() > 0) {
        result.mIsCollision = IsCollision::Yes;
    } else {
        if (validArea) {
            for (auto pieceRow = 0; pieceRow < pieceNumRows; ++pieceRow) {
                for (auto pieceColumn = 0; pieceColumn < pieceNumColumns; ++pieceColumn) {
                    auto& pieceCell = pieceGrid[pieceRow][pieceColumn];
                    if (pieceCell.mFirstSubCell.IsEmpty()) {
                        continue;
                    }

                    auto fieldRow = position.y + pieceRow;
                    auto fieldColumn = position.x + pieceColumn;
                    if (fieldRow < mLowestVisibleRow || fieldRow >= mNumRows || fieldColumn < 0 ||
                        fieldColumn >= mNumColumns) {

                        continue;
                    }

                    (*validArea)[fieldRow][fieldColumn] = validCell;
                }
            }
        }
    }
}

int Field::DetectFreeSpaceUp(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {0, 1};
    auto freePosition = ScanUntilNoCollision(pieceBlocks, position, step);
    return freePosition.y;
}

int Field::DetectFreeSpaceDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {0, -1};
    auto freePosition = ScanUntilNoCollision(pieceBlocks, position, step);
    return freePosition.y;
}

int Field::DetectFreeSpaceRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {1, 0};
    auto freePosition = ScanUntilNoCollision(pieceBlocks, position, step);
    return freePosition.x;
}

int Field::DetectFreeSpaceLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {-1, 0};
    auto freePosition = ScanUntilNoCollision(pieceBlocks, position, step);
    return freePosition.x;
}

Pht::IVec2 Field::ScanUntilNoCollision(const PieceBlocks& pieceBlocks,
                                       Pht::IVec2 position,
                                       const Pht::IVec2& step) const {
    auto numTries = 0;
    auto isScanStart = true;
    
    for (;;) {
        CheckCollision(mCollisionResult, pieceBlocks, position, step, isScanStart);
        if (mCollisionResult.mIsCollision != IsCollision::Yes) {
            break;
        }
        
        if (isScanStart) {
            isScanStart = false;
        }
        
        position += step;
        
        if (++numTries >= maxNumScanTries) {
            break;
        }
    }
    
    return position;
}

Field::CollisionPoints Field::GetOccupiedArea(const PieceBlocks& pieceBlocks,
                                              const Pht::IVec2& position) const {
    CollisionPoints collisions;
    auto pieceNumRows = pieceBlocks.mNumRows;
    auto pieceNumColumns = pieceBlocks.mNumColumns;

    for (auto pieceRow = 0; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto fieldRow = position.y + pieceRow;
            auto fieldColumn = position.x + pieceColumn;
            if (fieldRow < 0 || fieldRow >= mNumRows || fieldColumn < 0 ||
                fieldColumn >= mNumColumns) {
                collisions.PushBack(Pht::IVec2{pieceColumn, pieceRow});
                continue;
            }

            auto& fieldCell = mGrid[fieldRow][fieldColumn];
            if (!fieldCell.IsEmpty()) {
                collisions.PushBack(Pht::IVec2{pieceColumn, pieceRow});
            }
        }
    }

    return collisions;
}

Field::ImpactedBombs Field::DetectImpactedBombs(const PieceBlocks& pieceBlocks,
                                                const Pht::IVec2& position) const {
    ImpactedBombs impactedBombs;
    auto lowerPosition = position - Pht::IVec2{0, 1};
    
    CheckCollision(mCollisionResult, pieceBlocks, lowerPosition, Pht::IVec2{0, 0}, false);

    for (auto& collisionPointPieceCoord: mCollisionResult.mCollisionPoints) {
        Pht::IVec2 collisionPoint {collisionPointPieceCoord + lowerPosition};
        if (collisionPoint.x < 0 || collisionPoint.x >= mNumColumns ||
            collisionPoint.y >= mNumRows || collisionPoint.y < mLowestVisibleRow) {
            
            continue;
        }
        
        auto blockKind = mGrid[collisionPoint.y][collisionPoint.x].mFirstSubCell.mBlockKind;
        switch (blockKind) {
            case BlockKind::Bomb:
            case BlockKind::RowBomb:
                impactedBombs.PushBack({collisionPoint, blockKind});
                break;
            default:
                break;
        }
    }
    
    return impactedBombs;
}

Field::PieceFilledSlots Field::CalculatePieceFilledSlots(const FallingPiece& fallingPiece) {
    assert(mBlueprintGrid);

    PieceFilledSlots pieceFilledSlots;
    auto pieceBlocks = CreatePieceBlocks(fallingPiece);
    auto piecePosition = fallingPiece.GetIntPosition();
    auto pieceId = fallingPiece.GetId();

    for (auto pieceRow = 0; pieceRow < pieceBlocks.mNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceBlocks.mNumColumns; ++pieceColumn) {
            auto& pieceSubCell = pieceBlocks.mGrid[pieceRow][pieceColumn].mFirstSubCell;
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto row = piecePosition.y + pieceRow;
            auto column = piecePosition.x + pieceColumn;
            auto& fieldSubCell = mGrid[row][column].mFirstSubCell;
            
            if (fieldSubCell.mPieceId == pieceId) {
                auto& blueprintCell = (*mBlueprintGrid)[row][column];
                if (blueprintCell.mFill != Fill::Empty && IsCellAccordingToBlueprint(row, column)) {
                    PieceFilledSlot slot {
                        .mPosition = {column, row},
                        .mKind = PieceFilledSlotKind::Blueprint
                    };
                    pieceFilledSlots.PushBack(slot);
                } else if (blueprintCell.mFill == Fill::Empty) {
                    PieceFilledSlot slot {
                        .mPosition = {column, row},
                        .mKind = PieceFilledSlotKind::OutsideBlueprint
                    };
                    pieceFilledSlots.PushBack(slot);
                }
            }
        }
    }
    
    return pieceFilledSlots;
}

void Field::LandFallingPiece(const FallingPiece& fallingPiece, bool startBounceAnimation) {
    SetChanged();
    
    auto pieceBlocks = CreatePieceBlocks(fallingPiece);
    LandPieceBlocks(pieceBlocks,
                    fallingPiece.GetId(),
                    fallingPiece.GetIntPosition(),
                    true,
                    true,
                    startBounceAnimation);
}

void Field::LandPieceBlocks(const PieceBlocks& pieceBlocks,
                            int pieceId,
                            const Pht::IVec2& position,
                            bool updateCellPosition,
                            bool startBlueprintCellAnimation,
                            bool startBounceAnimation) {
    for (auto pieceRow = 0; pieceRow < pieceBlocks.mNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceBlocks.mNumColumns; ++pieceColumn) {
            auto& pieceCell = pieceBlocks.mGrid[pieceRow][pieceColumn];
            auto& pieceSubCell = pieceCell.mFirstSubCell;
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto row = position.y + pieceRow;
            auto column = position.x + pieceColumn;
            auto& fieldCell = mGrid[row][column];
            
            auto& fieldSubCell =
                fieldCell.mFirstSubCell.IsEmpty() ? fieldCell.mFirstSubCell : fieldCell.mSecondSubCell;
            
            fieldSubCell = pieceSubCell;
            fieldSubCell.mPieceId = pieceId;

            if (updateCellPosition) {
                fieldSubCell.mPosition = Pht::Vec2 {
                    static_cast<float>(column),
                    static_cast<float>(row)
                };
            }
            
            if (startBlueprintCellAnimation && mBlueprintGrid) {
                auto& blueprintCell = (*mBlueprintGrid)[row][column];
                if (blueprintCell.mFill != Fill::Empty && IsCellAccordingToBlueprint(row, column)) {
                    blueprintCell.mAnimation.mIsActive = true;
                }
            }
            
            if (startBounceAnimation) {
                auto& fallingBlockAnimation = fieldSubCell.mFallingBlockAnimation;
                fallingBlockAnimation.mState = FallingBlockAnimationComponent::State::Bouncing;
                fallingBlockAnimation.mVelocity = FallingBlockAnimationComponent::fallingPieceBounceVelocity;
            }
        }
    }
}

void Field::ManageBonds() {
    auto lowestVisibleRow = mLowestVisibleRow - 1;
    if (lowestVisibleRow < 0) {
        lowestVisibleRow = 0;
    }
    
    for (auto row = lowestVisibleRow; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            Pht::IVec2 position {column, row};
            
            MakeDiagonalBond(cell);
            MakeBonds(cell.mFirstSubCell, position);
            MakeBonds(cell.mSecondSubCell, position);
        }
    }
    
    for (auto row = lowestVisibleRow; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            Pht::IVec2 position {column, row};
            
            BreakRedundantBonds(cell.mFirstSubCell, position);
        }
    }
}

void Field::MakeDiagonalBond(Cell& cell) {
    auto& firstSubCell = cell.mFirstSubCell;
    auto& secondSubCell = cell.mSecondSubCell;

    if (!firstSubCell.IsEmpty() && !secondSubCell.IsEmpty() &&
        firstSubCell.mColor == secondSubCell.mColor && !firstSubCell.mBonds.mDiagonal &&
        !secondSubCell.mBonds.mDiagonal) {

        firstSubCell.mBonds.mDiagonal = true;
        secondSubCell.mBonds.mDiagonal = true;
        BondsAnimationSystem::StartBondAppearingAnimation(secondSubCell.mBondAnimations.mDiagonal);
    }
}

void Field::MakeBonds(SubCell& subCell, const Pht::IVec2& position) {
    if (subCell.IsEmpty() || subCell.mIsGrayLevelBlock || subCell.IsNonBlockObject()) {
        return;
    }
    
    auto& bonds = subCell.mBonds;
    if (!bonds.mUp && ShouldBeUpBond(subCell, position)) {
        bonds.mUp = true;
        BondsAnimationSystem::StartBondAppearingAnimation(subCell.mBondAnimations.mUp);
    }

    if (!bonds.mRight && ShouldBeRightBond(subCell, position)) {
        bonds.mRight = true;
        BondsAnimationSystem::StartBondAppearingAnimation(subCell.mBondAnimations.mRight);
    }
    
    if (!bonds.mDown && ShouldBeDownBond(subCell, position)) {
        bonds.mDown = true;
    }
    
    if (!bonds.mLeft && ShouldBeLeftBond(subCell, position)) {
        bonds.mLeft = true;
    }
}

bool Field::ShouldBeUpBond(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsUpperCellSide()) {
        return false;
    }
    
    auto upperRow = position.y + 1;
    if (upperRow >= mNumRows) {
        return false;
    }
    
    auto& upperCell = mGrid[upperRow][position.x];
    auto& firstUpperSubCell = upperCell.mFirstSubCell;
    auto& secondUpperSubCell = upperCell.mSecondSubCell;
    auto color = subCell.mColor;
    
    if (firstUpperSubCell.mColor == color && firstUpperSubCell.FillsLowerCellSide()) {
        return true;
    }
    
    if (secondUpperSubCell.mColor == color && secondUpperSubCell.FillsLowerCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeRightBond(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsRightCellSide()) {
        return false;
    }
    
    auto rightColumn = position.x + 1;
    if (rightColumn >= mNumColumns) {
        return false;
    }
    
    auto& cellToTheRight = mGrid[position.y][rightColumn];
    auto& firstRightSubCell = cellToTheRight.mFirstSubCell;
    auto& secondRightSubCell = cellToTheRight.mSecondSubCell;
    auto color = subCell.mColor;

    if (firstRightSubCell.mColor == color && firstRightSubCell.FillsLeftCellSide()) {
        return true;
    }
    
    if (secondRightSubCell.mColor == color && secondRightSubCell.FillsLeftCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeDownBond(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsLowerCellSide()) {
        return false;
    }
    
    auto lowerRow = position.y - 1;
    if (lowerRow < 0) {
        return false;
    }
    
    auto& lowerCell = mGrid[lowerRow][position.x];
    auto& firstLowerSubCell = lowerCell.mFirstSubCell;
    auto& secondLowerSubCell = lowerCell.mSecondSubCell;
    auto color = subCell.mColor;

    if (firstLowerSubCell.mColor == color && firstLowerSubCell.FillsUpperCellSide()) {
        return true;
    }
    
    if (secondLowerSubCell.mColor == color && secondLowerSubCell.FillsUpperCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeLeftBond(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsLeftCellSide()) {
        return false;
    }
    
    auto leftColumn = position.x - 1;
    if (leftColumn < 0) {
        return false;
    }
    
    auto& cellToTheLeft = mGrid[position.y][leftColumn];
    auto& firstLeftSubCell = cellToTheLeft.mFirstSubCell;
    auto& secondLeftSubCell = cellToTheLeft.mSecondSubCell;
    auto color = subCell.mColor;
    
    if (firstLeftSubCell.mColor == color && firstLeftSubCell.FillsRightCellSide()) {
        return true;
    }
    
    if (secondLeftSubCell.mColor == color && secondLeftSubCell.FillsRightCellSide()) {
        return true;
    }
    
    return false;
}

void Field::BreakRedundantBonds(SubCell& subCell, const Pht::IVec2& position) {
    if (subCell.IsEmpty() || subCell.mIsGrayLevelBlock || subCell.IsNonBlockObject()) {
        return;
    }

    auto& bonds = subCell.mBonds;
    if (bonds.mUpRight && UpRightBondWouldBeRedundant(subCell, position)) {
        bonds.mUpRight = false;
        BondsAnimationSystem::StartBondDisappearingAnimation(subCell.mBondAnimations.mUpRight);
    }
    
    if (bonds.mDownRight && DownRightBondWouldBeRedundant(subCell, position)) {
        bonds.mDownRight = false;
    }

    if (bonds.mDownLeft && DownLeftBondWouldBeRedundant(subCell, position)) {
        bonds.mDownLeft = false;
    }

    if (bonds.mUpLeft && UpLeftBondWouldBeRedundant(subCell, position)) {
        bonds.mUpLeft = false;
        BondsAnimationSystem::StartBondDisappearingAnimation(subCell.mBondAnimations.mUpLeft);
    }
}

bool Field::UpRightBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mBonds.mUp) {
        auto& upperSubCell = mGrid[position.y + 1][position.x].mFirstSubCell;
        if (upperSubCell.mBonds.mRight) {
            return true;
        }
    }

    if (subCell.mBonds.mRight) {
        auto& subCellToTheRight = mGrid[position.y][position.x + 1].mFirstSubCell;
        if (subCellToTheRight.mBonds.mUp) {
            return true;
        }
    }

    return false;
}

bool Field::DownRightBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mBonds.mDown) {
        auto& lowerSubCell = mGrid[position.y - 1][position.x].mFirstSubCell;
        if (lowerSubCell.mBonds.mRight) {
            return true;
        }
    }

    if (subCell.mBonds.mRight) {
        auto& subCellToTheRight = mGrid[position.y][position.x + 1].mFirstSubCell;
        if (subCellToTheRight.mBonds.mDown) {
            return true;
        }
    }

    return false;
}

bool Field::DownLeftBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mBonds.mDown) {
        auto& lowerSubCell = mGrid[position.y - 1][position.x].mFirstSubCell;
        if (lowerSubCell.mBonds.mLeft) {
            return true;
        }
    }

    if (subCell.mBonds.mLeft) {
        auto& subCellToTheLeft = mGrid[position.y][position.x - 1].mFirstSubCell;
        if (subCellToTheLeft.mBonds.mDown) {
            return true;
        }
    }

    return false;
}

bool Field::UpLeftBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mBonds.mUp) {
        auto& upperSubCell = mGrid[position.y + 1][position.x].mFirstSubCell;
        if (upperSubCell.mBonds.mLeft) {
            return true;
        }
    }

    if (subCell.mBonds.mLeft) {
        auto& subCellToTheLeft = mGrid[position.y][position.x - 1].mFirstSubCell;
        if (subCellToTheLeft.mBonds.mUp) {
            return true;
        }
    }

    return false;
}

void Field::MergeTriangleBlocksIntoCube(const Pht::IVec2& position) {
    auto& cell = GetCell(position);
    auto& firstSubCell = cell.mFirstSubCell;
    
    firstSubCell.mFill = Fill::Full;
    firstSubCell.mBlockKind = BlockKind::Full;
    
    auto& firstSubCellBonds = firstSubCell.mBonds;
    auto& secondSubCellBonds = cell.mSecondSubCell.mBonds;
    firstSubCellBonds.mDiagonal = false;
    
    if (secondSubCellBonds.mRight) {
        firstSubCellBonds.mRight = true;
    }

    if (secondSubCellBonds.mDownRight) {
        firstSubCellBonds.mDownRight = true;
    }

    if (secondSubCellBonds.mDown) {
        firstSubCellBonds.mDown = true;
    }

    if (secondSubCellBonds.mDownLeft) {
        firstSubCellBonds.mDownLeft = true;
    }

    if (secondSubCellBonds.mLeft) {
        firstSubCellBonds.mLeft = true;
    }

    if (secondSubCellBonds.mUpLeft) {
        firstSubCellBonds.mUpLeft = true;
    }

    if (secondSubCellBonds.mUp) {
        firstSubCellBonds.mUp = true;
    }

    if (secondSubCellBonds.mUpRight) {
        firstSubCellBonds.mUpRight = true;
    }
    
    auto& firstSubCellBondAnimations = firstSubCell.mBondAnimations;
    auto& secondSubCellBondAnimations = cell.mSecondSubCell.mBondAnimations;

    if (secondSubCellBondAnimations.mUpLeft.IsActive()) {
        firstSubCellBondAnimations.mUpLeft = secondSubCellBondAnimations.mUpLeft;
    }

    if (secondSubCellBondAnimations.mUp.IsActive()) {
        firstSubCellBondAnimations.mUp = secondSubCellBondAnimations.mUp;
    }

    if (secondSubCellBondAnimations.mUpRight.IsActive()) {
        firstSubCellBondAnimations.mUpRight = secondSubCellBondAnimations.mUpRight;
    }

    if (secondSubCellBondAnimations.mRight.IsActive()) {
        firstSubCellBondAnimations.mRight = secondSubCellBondAnimations.mRight;
    }

    cell.mSecondSubCell = SubCell {};
}

void Field::SetBlocksYPositionAndBounceFlag() {
    for (auto row = mLowestVisibleRow; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            
            auto& firstSubCell = cell.mFirstSubCell;
            firstSubCell.mPosition.y = row;
            firstSubCell.mFallingBlockAnimation.mShouldBounce = true;

            auto& secondSubCell = cell.mSecondSubCell;
            secondSubCell.mPosition.y = row;
            secondSubCell.mFallingBlockAnimation.mShouldBounce = true;
        }
    }
}

Field::RemovedSubCells Field::ClearFilledRows() {
    SetBlocksYPositionAndBounceFlag();
    SetChanged();
    
    RemovedSubCells removedSubCells;
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto rowIndex = mLowestVisibleRow; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        if (RowIsFull(mGrid[rowIndex])) {
            for (auto column = 0; column < mNumColumns; ++column) {
                auto& cell = mGrid[rowIndex][column];
                auto& firstSubCell = cell.mFirstSubCell;
                auto& secondSubCell = cell.mSecondSubCell;
                
                ProcessSubCell(removedSubCells, firstSubCell, rowIndex, column);
                ProcessSubCell(removedSubCells, secondSubCell, rowIndex, column);
                
                BreakCellDownBonds(rowIndex + 1, column);
                BreakCellUpBonds(rowIndex - 1, column);
                
                firstSubCell = SubCell {};
                firstSubCell.mBlockKind = BlockKind::ClearedRowBlock;
                secondSubCell = SubCell {};
                secondSubCell.mBlockKind = BlockKind::ClearedRowBlock;
            }
        }
    }

    return removedSubCells;
}

void Field::RemoveClearedRows() {
    SetChanged();
    
    RemovedSubCells removedSubCells;
    auto pastHighestVisibleRow = mLowestVisibleRow + GetNumRowsInOneScreen();
    
    for (auto rowIndex = mLowestVisibleRow; rowIndex < pastHighestVisibleRow;) {
        if (mGrid[rowIndex][0].mFirstSubCell.mBlockKind == BlockKind::ClearedRowBlock) {
            RemoveRowImpl(rowIndex, removedSubCells);
        } else {
            ++rowIndex;
        }
    }
}

void Field::RemoveRowImpl(int rowIndex, Field::RemovedSubCells& removedSubCells) {
    for (auto column = 0; column < mNumColumns; ++column) {
        auto& cell = mGrid[rowIndex][column];
        
        ProcessSubCell(removedSubCells, cell.mFirstSubCell, rowIndex, column);
        ProcessSubCell(removedSubCells, cell.mSecondSubCell, rowIndex, column);
        
        BreakCellDownBonds(rowIndex + 1, column);
        BreakCellUpBonds(rowIndex - 1, column);
        
        for (auto row = rowIndex; row < mNumRows - 1; ++row) {
            mGrid[row][column] = mGrid[row + 1][column];
        }
        
        mGrid[mNumRows - 1][column] = Cell {};
    }
}

void Field::BreakCellDownBonds(int row, int column) {
    if (row < mNumRows) {
        auto& cell = mGrid[row][column];
        BreakDownBonds(cell.mFirstSubCell);
        BreakDownBonds(cell.mSecondSubCell);
    }
}

void Field::BreakCellUpBonds(int row, int column) {
    if (row >= 0) {
        auto& cell = mGrid[row][column];
        BreakUpBonds(cell.mFirstSubCell);
        BreakUpBonds(cell.mSecondSubCell);
    }
}

void Field::BreakCellRightBonds(int row, int column) {
    if (column >= 0) {
        auto& cell = mGrid[row][column];
        BreakRightBonds(cell.mFirstSubCell);
        BreakRightBonds(cell.mSecondSubCell);
    }
}

void Field::BreakCellLeftBonds(int row, int column) {
    if (column < mNumColumns) {
        auto& cell = mGrid[row][column];
        BreakLeftBonds(cell.mFirstSubCell);
        BreakLeftBonds(cell.mSecondSubCell);
    }
}

void Field::BreakLowerLeftBond(int row, int column) {
    if (column < mNumColumns && row < mNumRows) {
        auto& cell = mGrid[row][column];
        cell.mFirstSubCell.mBonds.mDownLeft = false;
        cell.mSecondSubCell.mBonds.mDownLeft = false;
    }
}

void Field::BreakUpperLeftBond(int row, int column) {
    if (column < mNumColumns && row >= 0) {
        auto& cell = mGrid[row][column];
        cell.mFirstSubCell.mBonds.mUpLeft = false;
        cell.mSecondSubCell.mBonds.mUpLeft = false;
    }
}

void Field::BreakUpperRightBond(int row, int column) {
    if (column >= 0 && row >= 0) {
        auto& cell = mGrid[row][column];
        cell.mFirstSubCell.mBonds.mUpRight = false;
        cell.mSecondSubCell.mBonds.mUpRight = false;
    }
}

void Field::BreakLowerRightBond(int row, int column) {
    if (column >= 0 && row < mNumRows) {
        auto& cell = mGrid[row][column];
        cell.mFirstSubCell.mBonds.mDownRight = false;
        cell.mSecondSubCell.mBonds.mDownRight = false;
    }
}

Field::RemovedSubCells Field::RemoveRow(int rowIndex) {
    SetChanged();
    
    RemovedSubCells removedSubCells;
    RemoveRowImpl(rowIndex, removedSubCells);
    
    return removedSubCells;
}

Field::RemovedSubCells Field::RemoveAreaOfSubCells(const Pht::IVec2& areaPos,
                                                   const Pht::IVec2& areaSize,
                                                   bool removeCorners) {
    SetChanged();
    
    RemovedSubCells removedSubCells;
    
    for (auto row = areaPos.y; row < areaPos.y + areaSize.y; ++row) {
        for (auto column = areaPos.x; column < areaPos.x + areaSize.x; ++column) {
            if (row < mLowestVisibleRow || row >= mNumRows || column < 0 || column >= mNumColumns) {
                continue;
            }
            
            if (!removeCorners) {
                if (column == areaPos.x && row == areaPos.y) {
                    BreakCellRightBonds(row, column);
                    BreakCellUpBonds(row, column);
                    continue;
                }
                
                if (column == areaPos.x + areaSize.x - 1 && row == areaPos.y) {
                    BreakCellLeftBonds(row, column);
                    BreakCellUpBonds(row, column);
                    continue;
                }

                if (column == areaPos.x + areaSize.x - 1 && row == areaPos.y + areaSize.y - 1) {
                    BreakCellLeftBonds(row, column);
                    BreakCellDownBonds(row, column);
                    continue;
                }

                if (column == areaPos.x && row == areaPos.y + areaSize.y - 1) {
                    BreakCellRightBonds(row, column);
                    BreakCellDownBonds(row, column);
                    continue;
                }
            }
            
            if (column == areaPos.x) {
                BreakCellRightBonds(row, column - 1);
            }

            if (column == areaPos.x + areaSize.x - 1) {
                BreakCellLeftBonds(row, column + 1);
            }

            if (row == areaPos.y || row == mLowestVisibleRow) {
                BreakCellUpBonds(row - 1, column);
            }

            if (row == areaPos.y + areaSize.y - 1) {
                BreakCellDownBonds(row + 1, column);
            }
            
            auto& cell = mGrid[row][column];
            auto& firstSubCell = cell.mFirstSubCell;
            
            ProcessSubCell(removedSubCells, firstSubCell, row, column);
            ProcessSubCell(removedSubCells, cell.mSecondSubCell, row, column);
            
            if (!firstSubCell.IsAsteroid() &&
                firstSubCell.mBlockKind != BlockKind::ClearedRowBlock) {

                cell = Cell {};
            }
        }
    }
    
    if (removeCorners) {
        BreakLowerLeftBond(areaPos.y + areaSize.y, areaPos.x + areaSize.x);
        BreakUpperLeftBond(areaPos.y - 1, areaPos.x + areaSize.x);
        BreakUpperRightBond(areaPos.y - 1, areaPos.x - 1);
        BreakLowerRightBond(areaPos.y + areaSize.y, areaPos.x - 1);
    } else {
        BreakLowerLeftBond(areaPos.y + areaSize.y, areaPos.x + areaSize.x - 1);
        BreakLowerLeftBond(areaPos.y + areaSize.y - 1, areaPos.x + areaSize.x);
        BreakUpperLeftBond(areaPos.y, areaPos.x + areaSize.x);
        BreakUpperLeftBond(areaPos.y - 1, areaPos.x + areaSize.x - 1);
        BreakUpperRightBond(areaPos.y - 1, areaPos.x);
        BreakUpperRightBond(areaPos.y, areaPos.x - 1);
        BreakLowerRightBond(areaPos.y + areaSize.y - 1, areaPos.x - 1);
        BreakLowerRightBond(areaPos.y + areaSize.y, areaPos.x);
    }
    
    return removedSubCells;
}

Field::RemovedSubCells Field::RemoveAllNonEmptySubCells() {
    SetChanged();
    RemovedSubCells removedSubCells;

    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            if (cell.IsEmpty()) {
                continue;
            }
            
            ProcessSubCell(removedSubCells, cell.mFirstSubCell, row, column);
            ProcessSubCell(removedSubCells, cell.mSecondSubCell, row, column);
            cell = Cell {};
        }
    }
    
    return removedSubCells;
}

void Field::RemoveWholePiece(int pieceId, Field::RemovedSubCells& removedSubCells) {
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto& cell = mGrid[row][column];
            if (cell.IsEmpty()) {
                continue;
            }
            
            RemoveMatchingSubCell(pieceId, removedSubCells, cell.mFirstSubCell, row, column);
            RemoveMatchingSubCell(pieceId, removedSubCells, cell.mSecondSubCell, row, column);
        }
    }
}

void Field::RemoveMatchingSubCell(int pieceId,
                                  Field::RemovedSubCells& removedSubCells,
                                  SubCell& subCell,
                                  int row,
                                  int column) {
    if (subCell.mPieceId == pieceId) {
        SaveSubCellAndCancelFill(removedSubCells, subCell, row, column);
        
        subCell = SubCell {};
    }
}

void Field::ProcessSubCell(Field::RemovedSubCells& removedSubCells,
                           const SubCell& subCell,
                           int row,
                           int column) {
    SaveSubCellAndCancelFill(removedSubCells, subCell, row, column);
    
    if (subCell.mIsPartOfIndivisiblePiece) {
        RemoveWholePiece(subCell.mPieceId, removedSubCells);
    }
}

void Field::SaveSubCellAndCancelFill(Field::RemovedSubCells& removedSubCells,
                                     const SubCell& subCell,
                                     int row,
                                     int column) {
    auto position = subCell.mPosition;
    
    if (subCell.mBlockKind != BlockKind::None && subCell.mBlockKind != BlockKind::ClearedRowBlock &&
        row >= mLowestVisibleRow) {
        
        RemovedSubCell removedSubCell {
            .mExactPosition = subCell.mPosition,
            .mGridPosition = Pht::IVec2{column, row},
            .mRotation = subCell.mRotation,
            .mBlockKind = subCell.mBlockKind,
            .mColor = subCell.mColor,
            .mFlashingBlockAnimationState = subCell.mFlashingBlockAnimation.mState,
            .mPieceId = subCell.mPieceId,
            .mFlags.mIsGrayLevelBlock = subCell.mIsGrayLevelBlock,
            .mFlags.mIsAsteroidFragment = subCell.IsAsteroid(),
            .mFlags.mIsPulledDown = subCell.mIsPulledDown,
        };

        removedSubCells.PushBack(removedSubCell);
    }
    
    if (mBlueprintGrid) {
        auto& blueprintSlotFillAnimation =
            (*mBlueprintGrid)[static_cast<int>(position.y)][static_cast<int>(position.x)].mAnimation;
        
        if (blueprintSlotFillAnimation.mIsActive) {
            blueprintSlotFillAnimation = SlotFillAnimationComponent {};
        }
    }
}

void Field::RemovePiece(int pieceId,
                        const Pht::IVec2& position,
                        int pieceNumRows,
                        int pieceNumColumns) {
    auto xBegin = position.x >= 0 ? position.x : 0;
    auto xEnd = position.x + pieceNumColumns;
    if (xEnd > mNumColumns) {
        xEnd = mNumColumns;
    }

    auto yBegin = position.y >= 0 ? position.y : 0;
    auto yEnd = position.y + pieceNumRows;
    if (yEnd >= mNumRows) {
        yEnd = mNumRows;
    }

    for (auto row = yBegin; row < yEnd; ++row) {
        for (auto column = xBegin; column < xEnd; ++column) {
            auto& cell = mGrid[row][column];
            if (cell.IsEmpty()) {
                continue;
            }
            
            if (cell.mFirstSubCell.mPieceId == pieceId) {
                cell.mFirstSubCell = SubCell {};
            }

            if (cell.mSecondSubCell.mPieceId == pieceId) {
                cell.mSecondSubCell = SubCell {};
            }
        }
    }
}

FilledRowsResultWithPieceCells Field::MarkFilledRowsAndCountPieceCellsInFilledRows(int pieceId) {
    FilledRowsResultWithPieceCells result;
    auto pastHighestVisibleRow = mLowestVisibleRow + GetNumRowsInOneScreen();
    
    for (auto rowIndex = mLowestVisibleRow; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        auto& row = mGrid[rowIndex];
        if (RowIsFull(row)) {
            result.mFilledRowIndices.PushBack(rowIndex);
            
            for (auto& cell: row) {
                cell.mIsInFilledRow = true;
                result.mPieceCellsInFilledRows += CalculatePieceCellContribution(cell.mFirstSubCell,
                                                                                 pieceId);
                result.mPieceCellsInFilledRows += CalculatePieceCellContribution(cell.mSecondSubCell,
                                                                                 pieceId);
            }
        }
    }
    
    return result;
}

FilledRowsResultWithGrayLevelCells Field::MarkFilledRowsAndCountGrayLevelCellsInFilledRows() {
    FilledRowsResultWithGrayLevelCells result;
    auto pastHighestVisibleRow = mLowestVisibleRow + GetNumRowsInOneScreen();
    
    for (auto rowIndex = mLowestVisibleRow; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        auto& row = mGrid[rowIndex];
        if (RowIsFull(row)) {
            result.mFilledRowIndices.PushBack(rowIndex);
            
            for (auto& cell: row) {
                cell.mIsInFilledRow = true;
                result.mGrayLevelCellsInFilledRows += CalculateGrayLevelCellContribution(cell.mFirstSubCell);
                result.mGrayLevelCellsInFilledRows += CalculateGrayLevelCellContribution(cell.mSecondSubCell);
            }
        }
    }
    
    return result;
}

void Field::UnmarkFilledRows(const FilledRowIndices& filledRowIndices) {
    for (auto i = 0; i < filledRowIndices.Size(); ++i) {
        auto& row = mGrid[filledRowIndices.At(i)];
        for (auto& cell: row) {
            cell.mIsInFilledRow = false;
        }
    }
}
