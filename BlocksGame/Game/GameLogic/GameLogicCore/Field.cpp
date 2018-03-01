#include "Field.hpp"

#include <assert.h>

// Game includes.
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "Level.hpp"
#include "CollisionDetection.hpp"
#include "WeldsAnimation.hpp"

using namespace BlocksGame;

namespace {
    const auto maxNumScanTries {Field::maxNumRows};
    const auto maxNumRowsInOneScreen {19};
    const SubCell fullSubCell {Fill::Full};

    bool RowIsFull(const std::vector<Cell>& row) {
        for (auto& cell: row) {
            if (!cell.IsFull()) {
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

    void BreakDownWelds(Welds& welds) {
        welds.mDownRight = false;
        welds.mDown = false;
        welds.mDownLeft = false;
    }
    
    void BreakUpWelds(Welds& welds) {
        welds.mUpRight = false;
        welds.mUp = false;
        welds.mUpLeft = false;
        
        auto& animations {welds.mAnimations};
        animations.mUp = WeldAnimation {};
        animations.mUpRight = WeldAnimation {};
        animations.mUpLeft = WeldAnimation {};
    }
    
    void BreakRightWelds(Welds& welds) {
        welds.mUpRight = false;
        welds.mRight = false;
        welds.mDownRight = false;
        
        auto& animations {welds.mAnimations};
        animations.mUpRight = WeldAnimation {};
        animations.mRight = WeldAnimation {};
    }
    
    void BreakLeftWelds(Welds& welds) {
        welds.mDownLeft = false;
        welds.mLeft = false;
        welds.mUpLeft = false;
    }
    
    PieceBlocks CreatePieceBlocks(const FallingPiece& fallingPiece) {
        auto& pieceType {fallingPiece.GetPieceType()};
    
        return {
            pieceType.GetGrid(fallingPiece.GetRotation()),
            pieceType.GetGridNumRows(),
            pieceType.GetGridNumColumns()
        };
    }
    
    int XMin(const Field::PieceBlockCoords& pieceBlockCoords) {
        auto xMin {Field::maxNumColumns - 1};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto x {pieceBlockCoords.At(i).mPosition.x};
            
            if (x < xMin) {
                xMin = x;
            }
        }
        
        return xMin;
    }
    
    int XMax(const Field::PieceBlockCoords& pieceBlockCoords) {
        auto xMax {0};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto x {pieceBlockCoords.At(i).mPosition.x};
            
            if (x > xMax) {
                xMax = x;
            }
        }
        
        return xMax;
    }

    int YMin(const Field::PieceBlockCoords& pieceBlockCoords) {
        auto yMin {10000};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto y {pieceBlockCoords.At(i).mPosition.y};
            
            if (y < yMin) {
                yMin = y;
            }
        }
        
        return yMin;
    }
    
    int YMax(const Field::PieceBlockCoords& pieceBlockCoords) {
        auto yMax {0};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto y {pieceBlockCoords.At(i).mPosition.y};
            
            if (y > yMax) {
                yMax = y;
            }
        }
        
        return yMax;
    }
}

Field::Field() {
    std::vector<Cell> emptyRow(maxNumColumns);
    
    for (auto i {0}; i < maxNumRows; ++i) {
        mPieceBlockGrid.push_back(emptyRow);
    }
}

void Field::Init(const Level& level) {
    mBlueprintGrid = nullptr;
    mNumColumns = level.GetNumColumns();
    mNumRows = level.GetNumRows();
    
    if (auto* clearGrid {level.GetClearGrid()}) {
        mGrid = *clearGrid;
    } else {
        auto* blueprintGrid {level.GetBlueprintGrid()};
        assert(blueprintGrid);
        
        mBlueprintGrid = std::make_unique<BlueprintCellGrid>(*blueprintGrid);
        mGrid.clear();
        mGrid.reserve(mNumRows);
        
        std::vector<Cell> emptyRow(mNumColumns);
        
        for (auto rowIndex {0}; rowIndex < mNumRows; ++rowIndex) {
            mGrid.push_back(emptyRow);
        }
    }
    
    mPreviousGrid = mGrid;
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

void Field::CopyGridNoAlloc(CellGrid& to, const CellGrid& from) {
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
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
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto rowIndex {mLowestVisibleRow}; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        if (RowIsFull(mGrid[rowIndex])) {
            return true;
        }
    }
    
    return false;
}

int Field::CalculateNumLevelBlocks() const {
    auto result {0};
    
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            
            if (cell.mFirstSubCell.mIsLevel || cell.mSecondSubCell.mIsLevel) {
                ++result;
            }
        }
    }
    
    return result;
}

int Field::CalculateNumEmptyBlueprintSlots() const {
    assert(mBlueprintGrid);
    auto result {0};
    
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            if ((*mBlueprintGrid)[row][column].mFill != Fill::Empty &&
                !IsCellAccordingToBlueprint(row, column)) {
                ++result;
            }
        }
    }

    return result;
}

int Field::CalculateHighestLevelBlock() const {
    for (auto row {mNumRows - 1}; row >= 0; --row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            if (mGrid[row][column].mFirstSubCell.mIsLevel) {
                return row;
            }
        }
    }

    return 0;
}

int Field::AccordingToBlueprintHeight() const {
    assert(mBlueprintGrid);
    
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            if (!IsCellAccordingToBlueprint(row, column)) {
                return row;
            }
        }
    }

    return mNumRows;
}

bool Field::IsCellAccordingToBlueprint(int row, int column) const {
    auto& cell {mGrid[row][column]};
    auto blueprintFill {(*mBlueprintGrid)[row][column].mFill};
    
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

int Field::DetectCollisionDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {0, -1};
    auto collisionPosition {ScanUntilCollision(pieceBlocks, position, step)};
    return collisionPosition.y + 1;
}

int Field::DetectCollisionRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {1, 0};
    auto collisionPosition {ScanUntilCollision(pieceBlocks, position, step)};
    return collisionPosition.x - 1;
}

int Field::DetectCollisionLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {-1, 0};
    auto collisionPosition {ScanUntilCollision(pieceBlocks, position, step)};
    return collisionPosition.x + 1;
}

Pht::IVec2 Field::ScanUntilCollision(const PieceBlocks& pieceBlocks,
                                     Pht::IVec2 position,
                                     const Pht::IVec2& step) const {
    auto isScanStart {true};
    
    for (;;) {
        CheckCollision(mCollisionResult, pieceBlocks, position, step, isScanStart);
        
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
                           bool isScanStart) const {
    result.mIsCollision = IsCollision::No;
    result.mCollisionPoints.Clear();
    
    auto pieceNumRows {pieceBlocks.mNumRows};
    auto pieceNumColumns {pieceBlocks.mNumColumns};
    auto& pieceGrid {pieceBlocks.mGrid};

    for (auto pieceRow {0}; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto& pieceCell {pieceGrid[pieceRow][pieceColumn]};
            
            auto& pieceSubCell {
                pieceCell.mSecondSubCell.IsEmpty() ? pieceCell.mFirstSubCell : fullSubCell
            };
            
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto fieldRow {position.y + pieceRow};
            auto fieldColumn {position.x + pieceColumn};

            if (fieldRow < mLowestVisibleRow || fieldRow >= mNumRows || fieldColumn < 0 ||
                fieldColumn >= mNumColumns) {
                result.mCollisionPoints.PushBack(Pht::IVec2{pieceColumn, pieceRow});
                continue;
            }

            auto& fieldCell {mGrid[fieldRow][fieldColumn]};
            
            auto firstSubCellIntersects {
                CollisionDetection::SubCellsIntersect(fieldCell.mFirstSubCell,
                                                      pieceSubCell,
                                                      scanDirection,
                                                      isScanStart)
            };
            
            auto secondSubCellIntersects {
                CollisionDetection::SubCellsIntersect(fieldCell.mSecondSubCell,
                                                      pieceSubCell,
                                                      scanDirection,
                                                      isScanStart)
            };
            
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
    }
}

int Field::DetectFreeSpaceUp(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {0, 1};
    auto freePosition {ScanUntilNoCollision(pieceBlocks, position, step)};
    return freePosition.y;
}

int Field::DetectFreeSpaceDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {0, -1};
    auto freePosition {ScanUntilNoCollision(pieceBlocks, position, step)};
    return freePosition.y;
}

int Field::DetectFreeSpaceRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {1, 0};
    auto freePosition {ScanUntilNoCollision(pieceBlocks, position, step)};
    return freePosition.x;
}

int Field::DetectFreeSpaceLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const {
    Pht::IVec2 step {-1, 0};
    auto freePosition {ScanUntilNoCollision(pieceBlocks, position, step)};
    return freePosition.x;
}

Pht::IVec2 Field::ScanUntilNoCollision(const PieceBlocks& pieceBlocks,
                                       Pht::IVec2 position,
                                       const Pht::IVec2& step) const {
    auto numTries {0};
    auto isScanStart {true};
    
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
    auto pieceNumRows {pieceBlocks.mNumRows};
    auto pieceNumColumns {pieceBlocks.mNumColumns};

    for (auto pieceRow {0}; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto fieldRow {position.y + pieceRow};
            auto fieldColumn {position.x + pieceColumn};

            if (fieldRow < 0 || fieldRow >= mNumRows || fieldColumn < 0 ||
                fieldColumn >= mNumColumns) {
                collisions.PushBack(Pht::IVec2{pieceColumn, pieceRow});
                continue;
            }

            auto& fieldCell {mGrid[fieldRow][fieldColumn]};
            
            if (!fieldCell.IsEmpty()) {
                collisions.PushBack(Pht::IVec2{pieceColumn, pieceRow});
            }
        }
    }

    return collisions;
}

void Field::LandFallingPiece(const FallingPiece& fallingPiece) {
    SetChanged();
    SaveState();
    
    auto pieceBlocks {CreatePieceBlocks(fallingPiece)};
    LandPieceBlocks(pieceBlocks, fallingPiece.GetId(), fallingPiece.GetIntPosition(), true, true);
    ManageWelds();
}

void Field::LandPieceBlocks(const PieceBlocks& pieceBlocks,
                            int pieceId,
                            const Pht::IVec2& position,
                            bool updateCellPosition,
                            bool startBlueprintCellAnimation) {
    for (auto pieceRow {0}; pieceRow < pieceBlocks.mNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceBlocks.mNumColumns; ++pieceColumn) {
            auto& pieceCell {pieceBlocks.mGrid[pieceRow][pieceColumn]};
            auto& pieceSubCell {pieceCell.mFirstSubCell};
            
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto row {position.y + pieceRow};
            auto column {position.x + pieceColumn};
            auto& fieldCell {mGrid[row][column]};
            
            auto& fieldSubCell {
                fieldCell.mFirstSubCell.IsEmpty() ? fieldCell.mFirstSubCell : fieldCell.mSecondSubCell
            };
            
            fieldSubCell = pieceSubCell;
            fieldSubCell.mPieceId = pieceId;

            if (updateCellPosition) {
                fieldSubCell.mPosition = Pht::Vec2 {
                    static_cast<float>(column),
                    static_cast<float>(row)
                };
            }
            
            if (startBlueprintCellAnimation && mBlueprintGrid) {
                auto& blueprintCell {(*mBlueprintGrid)[row][column]};
                if (blueprintCell.mFill != Fill::Empty && IsCellAccordingToBlueprint(row, column)) {
                    blueprintCell.mAnimation.mIsActive = true;
                }
            }
        }
    }
}

void Field::ManageWelds() {
    auto lowestVisibleRow {mLowestVisibleRow - 1};
    
    if (lowestVisibleRow < 0) {
        lowestVisibleRow = 0;
    }
    
    for (auto row {lowestVisibleRow}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            Pht::IVec2 position {column, row};
            
            MakeDiagonalWeld(cell);
            MakeWelds(cell.mFirstSubCell, position);
            MakeWelds(cell.mSecondSubCell, position);
        }
    }
    
    for (auto row {lowestVisibleRow}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            Pht::IVec2 position {column, row};
            
            BreakRedundantWelds(cell.mFirstSubCell, position);
        }
    }
}

void Field::MakeDiagonalWeld(Cell& cell) {
    auto& firstSubCell {cell.mFirstSubCell};
    auto& secondSubCell {cell.mSecondSubCell};
    
    if (!firstSubCell.IsEmpty() && !secondSubCell.IsEmpty() &&
        firstSubCell.mColor == secondSubCell.mColor && !firstSubCell.mWelds.mDiagonal &&
        !secondSubCell.mWelds.mDiagonal) {
        firstSubCell.mWelds.mDiagonal = true;
        secondSubCell.mWelds.mDiagonal = true;
        WeldsAnimation::StartWeldAppearingAnimation(secondSubCell.mWelds.mAnimations.mDiagonal);
    }
}

void Field::MakeWelds(SubCell& subCell, const Pht::IVec2& position) {
    if (subCell.IsEmpty() || subCell.mIsLevel) {
        return;
    }
    
    auto& welds {subCell.mWelds};

    if (!welds.mUp && ShouldBeUpWeld(subCell, position)) {
        welds.mUp = true;
        WeldsAnimation::StartWeldAppearingAnimation(welds.mAnimations.mUp);
    }

    if (!welds.mRight && ShouldBeRightWeld(subCell, position)) {
        welds.mRight = true;
        WeldsAnimation::StartWeldAppearingAnimation(welds.mAnimations.mRight);
    }
    
    if (!welds.mDown && ShouldBeDownWeld(subCell, position)) {
        welds.mDown = true;
    }
    
    if (!welds.mLeft && ShouldBeLeftWeld(subCell, position)) {
        welds.mLeft = true;
    }
}

bool Field::ShouldBeUpWeld(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsUpperCellSide()) {
        return false;
    }
    
    auto upperRow {position.y + 1};
    
    if (upperRow >= mNumRows) {
        return false;
    }
    
    auto& upperCell {mGrid[upperRow][position.x]};
    auto& firstUpperSubCell {upperCell.mFirstSubCell};
    auto& secondUpperSubCell {upperCell.mSecondSubCell};
    auto color {subCell.mColor};
    
    if (firstUpperSubCell.mColor == color && firstUpperSubCell.FillsLowerCellSide()) {
        return true;
    }
    
    if (secondUpperSubCell.mColor == color && secondUpperSubCell.FillsLowerCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeRightWeld(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsRightCellSide()) {
        return false;
    }
    
    auto rightColumn {position.x + 1};
    
    if (rightColumn >= mNumColumns) {
        return false;
    }
    
    auto& cellToTheRight {mGrid[position.y][rightColumn]};
    auto& firstRightSubCell {cellToTheRight.mFirstSubCell};
    auto& secondRightSubCell {cellToTheRight.mSecondSubCell};
    auto color {subCell.mColor};

    if (firstRightSubCell.mColor == color && firstRightSubCell.FillsLeftCellSide()) {
        return true;
    }
    
    if (secondRightSubCell.mColor == color && secondRightSubCell.FillsLeftCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeDownWeld(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsLowerCellSide()) {
        return false;
    }
    
    auto lowerRow {position.y - 1};
    
    if (lowerRow < 0) {
        return false;
    }
    
    auto& lowerCell {mGrid[lowerRow][position.x]};
    auto& firstLowerSubCell {lowerCell.mFirstSubCell};
    auto& secondLowerSubCell {lowerCell.mSecondSubCell};
    auto color {subCell.mColor};

    if (firstLowerSubCell.mColor == color && firstLowerSubCell.FillsUpperCellSide()) {
        return true;
    }
    
    if (secondLowerSubCell.mColor == color && secondLowerSubCell.FillsUpperCellSide()) {
        return true;
    }
    
    return false;
}

bool Field::ShouldBeLeftWeld(const SubCell& subCell, const Pht::IVec2& position) const {
    if (!subCell.FillsLeftCellSide()) {
        return false;
    }
    
    auto leftColumn {position.x - 1};
    
    if (leftColumn < 0) {
        return false;
    }
    
    auto& cellToTheLeft {mGrid[position.y][leftColumn]};
    auto& firstLeftSubCell {cellToTheLeft.mFirstSubCell};
    auto& secondLeftSubCell {cellToTheLeft.mSecondSubCell};
    auto color {subCell.mColor};
    
    if (firstLeftSubCell.mColor == color && firstLeftSubCell.FillsRightCellSide()) {
        return true;
    }
    
    if (secondLeftSubCell.mColor == color && secondLeftSubCell.FillsRightCellSide()) {
        return true;
    }
    
    return false;
}

void Field::BreakRedundantWelds(SubCell& subCell, const Pht::IVec2& position) {
    if (subCell.IsEmpty() || subCell.mIsLevel) {
        return;
    }

    auto& welds {subCell.mWelds};
    
    if (welds.mUpRight && UpRightWeldWouldBeRedundant(subCell, position)) {
        welds.mUpRight = false;
        WeldsAnimation::StartWeldDisappearingAnimation(welds.mAnimations.mUpRight);
    }
    
    if (welds.mDownRight && DownRightWeldWouldBeRedundant(subCell, position)) {
        welds.mDownRight = false;
    }

    if (welds.mDownLeft && DownLeftWeldWouldBeRedundant(subCell, position)) {
        welds.mDownLeft = false;
    }

    if (welds.mUpLeft && UpLeftWeldWouldBeRedundant(subCell, position)) {
        welds.mUpLeft = false;
        WeldsAnimation::StartWeldDisappearingAnimation(welds.mAnimations.mUpLeft);
    }
}

bool Field::UpRightWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mWelds.mUp) {
        auto& upperSubCell {mGrid[position.y + 1][position.x].mFirstSubCell};
        
        if (upperSubCell.mWelds.mRight) {
            return true;
        }
    }

    if (subCell.mWelds.mRight) {
        auto& subCellToTheRight {mGrid[position.y][position.x + 1].mFirstSubCell};
        
        if (subCellToTheRight.mWelds.mUp) {
            return true;
        }
    }

    return false;
}

bool Field::DownRightWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mWelds.mDown) {
        auto& lowerSubCell {mGrid[position.y - 1][position.x].mFirstSubCell};
        
        if (lowerSubCell.mWelds.mRight) {
            return true;
        }
    }

    if (subCell.mWelds.mRight) {
        auto& subCellToTheRight {mGrid[position.y][position.x + 1].mFirstSubCell};
        
        if (subCellToTheRight.mWelds.mDown) {
            return true;
        }
    }

    return false;
}

bool Field::DownLeftWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mWelds.mDown) {
        auto& lowerSubCell {mGrid[position.y - 1][position.x].mFirstSubCell};
        
        if (lowerSubCell.mWelds.mLeft) {
            return true;
        }
    }

    if (subCell.mWelds.mLeft) {
        auto& subCellToTheLeft {mGrid[position.y][position.x - 1].mFirstSubCell};
        
        if (subCellToTheLeft.mWelds.mDown) {
            return true;
        }
    }

    return false;
}

bool Field::UpLeftWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const {
    if (subCell.mWelds.mUp) {
        auto& upperSubCell {mGrid[position.y + 1][position.x].mFirstSubCell};
        
        if (upperSubCell.mWelds.mLeft) {
            return true;
        }
    }

    if (subCell.mWelds.mLeft) {
        auto& subCellToTheLeft {mGrid[position.y][position.x - 1].mFirstSubCell};
        
        if (subCellToTheLeft.mWelds.mUp) {
            return true;
        }
    }

    return false;
}

void Field::MergeTriangleBlocksIntoCube(const Pht::IVec2& position) {
    auto& cell {GetCell(position)};
    auto& firstSubCell {cell.mFirstSubCell};
    
    firstSubCell.mFill = Fill::Full;
    firstSubCell.mBlockRenderableKind = BlockRenderableKind::Full;
    
    auto& firstSubCellWelds {firstSubCell.mWelds};
    auto& secondSubCellWelds {cell.mSecondSubCell.mWelds};
    firstSubCellWelds.mDiagonal = false;
    
    if (secondSubCellWelds.mRight) {
        firstSubCellWelds.mRight = true;
    }

    if (secondSubCellWelds.mDownRight) {
        firstSubCellWelds.mDownRight = true;
    }

    if (secondSubCellWelds.mDown) {
        firstSubCellWelds.mDown = true;
    }

    if (secondSubCellWelds.mDownLeft) {
        firstSubCellWelds.mDownLeft = true;
    }

    if (secondSubCellWelds.mLeft) {
        firstSubCellWelds.mLeft = true;
    }

    if (secondSubCellWelds.mUpLeft) {
        firstSubCellWelds.mUpLeft = true;
    }

    if (secondSubCellWelds.mUp) {
        firstSubCellWelds.mUp = true;
    }

    if (secondSubCellWelds.mUpRight) {
        firstSubCellWelds.mUpRight = true;
    }
    
    auto& firstSubCellWeldAnimations {firstSubCellWelds.mAnimations};
    auto& secondSubCellWeldAnimations {secondSubCellWelds.mAnimations};

    if (secondSubCellWeldAnimations.mUpLeft.IsActive()) {
        firstSubCellWeldAnimations.mUpLeft = secondSubCellWeldAnimations.mUpLeft;
    }

    if (secondSubCellWeldAnimations.mUp.IsActive()) {
        firstSubCellWeldAnimations.mUp = secondSubCellWeldAnimations.mUp;
    }

    if (secondSubCellWeldAnimations.mUpRight.IsActive()) {
        firstSubCellWeldAnimations.mUpRight = secondSubCellWeldAnimations.mUpRight;
    }

    if (secondSubCellWeldAnimations.mRight.IsActive()) {
        firstSubCellWeldAnimations.mRight = secondSubCellWeldAnimations.mRight;
    }

    cell.mSecondSubCell = SubCell {};
}

void Field::PullDownLoosePieces() {
    SetChanged();
    
    for (auto row {mLowestVisibleRow}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            PullDownPiece(row, column, ScanDirection::Right);
        }
        
        for (auto column {mNumColumns - 1}; column >= 0; --column) {
            PullDownPiece(row, column, ScanDirection::Left);
        }
    }
    
    ResetAllCellsTriedScanDirection();
}

void Field::PullDownPiece(int row, int column, ScanDirection scanDirection) {
    auto& cell {mGrid[row][column]};
    Pht::IVec2 position {column, row};
    
    PullDownPiece(cell.mFirstSubCell, position, scanDirection);
    PullDownPiece(cell.mSecondSubCell, position, scanDirection);
}

void Field::PullDownPiece(const SubCell& subCell,
                          const Pht::IVec2& scanPosition,
                          ScanDirection scanDirection) {
    if (subCell.IsEmpty() || subCell.mIsLevel || subCell.mTriedScanDirection == scanDirection) {
        return;
    }
    
    Pht::IVec2 piecePosition {0, 0};
    auto pieceBlocks {
        ExtractPieceBlocks(piecePosition, subCell.mColor, scanPosition, scanDirection)
    };

    Pht::IVec2 step {0, -1};
    auto collisionPosition {ScanUntilCollision(pieceBlocks, piecePosition, step)};
    
    if (collisionPosition.y + 1 >= mLowestVisibleRow) {
        LandPulledDownPieceBlocks(pieceBlocks, collisionPosition + Pht::IVec2 {0, 1});
    } else {
        LandPulledDownPieceBlocks(pieceBlocks, piecePosition);
    }
}

PieceBlocks Field::ExtractPieceBlocks(Pht::IVec2& piecePosition,
                                      BlockColor color,
                                      const Pht::IVec2& scanPosition,
                                      ScanDirection scanDirection) {
    mPieceBlockCoords.Clear();
    FindPieceBlocks(color, scanPosition);
    
    piecePosition.x = XMin(mPieceBlockCoords);
    piecePosition.y = YMin(mPieceBlockCoords);

    ClearPieceBlockGrid();
    
    for (auto i {0}; i < mPieceBlockCoords.Size(); ++i) {
        auto& coord {mPieceBlockCoords.At(i)};
        auto& position {coord.mPosition};
        auto pieceRow {position.y - piecePosition.y};
        auto pieceColumn {position.x - piecePosition.x};
        
        auto& cell {mGrid[position.y][position.x]};
        auto& fieldSubCell {coord.mIsFirstSubCell ? cell.mFirstSubCell : cell.mSecondSubCell};
        
        fieldSubCell.mIsFound = false;
        fieldSubCell.mTriedScanDirection = scanDirection;
        
        auto& pieceBlockCell {mPieceBlockGrid[pieceRow][pieceColumn]};
        
        if (pieceBlockCell.mFirstSubCell.IsEmpty()) {
            pieceBlockCell.mFirstSubCell = fieldSubCell;
        } else {
            pieceBlockCell.mSecondSubCell = fieldSubCell;
        }
        
        fieldSubCell = SubCell {};
    }

    return PieceBlocks {
        mPieceBlockGrid,
        YMax(mPieceBlockCoords) - piecePosition.y + 1,
        XMax(mPieceBlockCoords) - piecePosition.x + 1
    };
}

void Field::FindPieceBlocks(BlockColor color, const Pht::IVec2& position) {
    auto& cell {mGrid[position.y][position.x]};
    auto& firstSubCell {cell.mFirstSubCell};
    auto& secondSubCell {cell.mSecondSubCell};
    SubCell* subCell {nullptr};
    auto isFirstSubCell {true};
    
    if (firstSubCell.mColor == color && !firstSubCell.mIsFound) {
        subCell = &firstSubCell;
    } else if (secondSubCell.mColor == color && !secondSubCell.mIsFound) {
        subCell = &secondSubCell;
        isFirstSubCell = false;
    } else {
        return;
    }
    
    PieceBlockCoord coord {position, isFirstSubCell};
    mPieceBlockCoords.PushBack(coord);
    
    subCell->mIsFound = true;
    auto& welds {subCell->mWelds};
    
    if (welds.mDiagonal) {
        FindPieceBlocks(color, position);
    }
    
    if (welds.mUp) {
        FindPieceBlocks(color, position + Pht::IVec2 {0, 1});
    }

    if (welds.mUpRight) {
        FindPieceBlocks(color, position + Pht::IVec2 {1, 1});
    }
    
    if (welds.mRight) {
        FindPieceBlocks(color, position + Pht::IVec2 {1, 0});
    }
    
    if (welds.mDownRight) {
        FindPieceBlocks(color, position + Pht::IVec2 {1, -1});
    }

    if (welds.mDown) {
        FindPieceBlocks(color, position + Pht::IVec2 {0, -1});
    }

    if (welds.mDownLeft) {
        FindPieceBlocks(color, position + Pht::IVec2 {-1, -1});
    }

    if (welds.mLeft) {
        FindPieceBlocks(color, position + Pht::IVec2 {-1, 0});
    }

    if (welds.mUpLeft) {
        FindPieceBlocks(color, position + Pht::IVec2 {-1, 1});
    }
}

void Field::ResetAllCellsTriedScanDirection() {
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            cell.mFirstSubCell.mTriedScanDirection = ScanDirection::None;
            cell.mSecondSubCell.mTriedScanDirection = ScanDirection::None;
        }
    }
}

void Field::ClearPieceBlockGrid() {
    for (auto row {0}; row < maxNumRows; ++row) {
        for (auto column {0}; column < maxNumColumns; ++column) {
            mPieceBlockGrid[row][column] = Cell {};
        }
    }
}

void Field::LandPulledDownPieceBlocks(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) {
    for (auto pieceRow {0}; pieceRow < pieceBlocks.mNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceBlocks.mNumColumns; ++pieceColumn) {
            auto& pieceCell {pieceBlocks.mGrid[pieceRow][pieceColumn]};
            
            if (pieceCell.IsEmpty()) {
                continue;
            }
            
            auto row {position.y + pieceRow};
            auto column {position.x + pieceColumn};
            auto& fieldCell {mGrid[row][column]};
            
            if (pieceCell.mSecondSubCell.IsEmpty()) {
                auto& fieldSubCell {
                    fieldCell.mFirstSubCell.IsEmpty() ?
                    fieldCell.mFirstSubCell : fieldCell.mSecondSubCell
                };
                
                fieldSubCell = pieceCell.mFirstSubCell;
            } else {
                fieldCell = pieceCell;
            }
        }
    }
}

Field::RemovedSubCells Field::RemoveFilledRows() {
    SetChanged();
    
    RemovedSubCells removedSubCells;
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto rowIndex {mLowestVisibleRow}; rowIndex < pastHighestVisibleRow;) {
        if (RowIsFull(mGrid[rowIndex])) {
            RemoveRowImpl(rowIndex, removedSubCells);
        } else {
            ++rowIndex;
        }
    }
    
    return removedSubCells;
}

void Field::RemoveRowImpl(int rowIndex, Field::RemovedSubCells& removedSubCells) {
    for (auto column {0}; column < mNumColumns; ++column) {
        auto& cell {mGrid[rowIndex][column]};
        
        ProcessSubCell(removedSubCells, cell.mFirstSubCell);
        ProcessSubCell(removedSubCells, cell.mSecondSubCell);
        
        BreakCellDownWelds(rowIndex + 1, column);
        BreakCellUpWelds(rowIndex - 1, column);
        
        for (auto row {rowIndex}; row < mNumRows - 1; ++row) {
            mGrid[row][column] = mGrid[row + 1][column];
        }
        
        mGrid[mNumRows - 1][column] = Cell {};
    }
}

void Field::BreakCellDownWelds(int row, int column) {
    if (row < mNumRows) {
        auto& cell {mGrid[row][column]};
        BreakDownWelds(cell.mFirstSubCell.mWelds);
        BreakDownWelds(cell.mSecondSubCell.mWelds);
    }
}

void Field::BreakCellUpWelds(int row, int column) {
    if (row >= 0) {
        auto& cell {mGrid[row][column]};
        BreakUpWelds(cell.mFirstSubCell.mWelds);
        BreakUpWelds(cell.mSecondSubCell.mWelds);
    }
}

void Field::BreakCellRightWelds(int row, int column) {
    if (column >= 0) {
        auto& cell {mGrid[row][column]};
        BreakRightWelds(cell.mFirstSubCell.mWelds);
        BreakRightWelds(cell.mSecondSubCell.mWelds);
    }
}

void Field::BreakCellLeftWelds(int row, int column) {
    if (column < mNumColumns) {
        auto& cell {mGrid[row][column]};
        BreakLeftWelds(cell.mFirstSubCell.mWelds);
        BreakLeftWelds(cell.mSecondSubCell.mWelds);
    }
}

void Field::BreakLowerLeftWeld(int row, int column) {
    if (column < mNumColumns && row < mNumRows) {
        auto& cell {mGrid[row][column]};
        cell.mFirstSubCell.mWelds.mDownLeft = false;
        cell.mSecondSubCell.mWelds.mDownLeft = false;
    }
}

void Field::BreakUpperLeftWeld(int row, int column) {
    if (column < mNumColumns && row >= 0) {
        auto& cell {mGrid[row][column]};
        cell.mFirstSubCell.mWelds.mUpLeft = false;
        cell.mSecondSubCell.mWelds.mUpLeft = false;
    }
}

void Field::BreakUpperRightWeld(int row, int column) {
    if (column >= 0 && row >= 0) {
        auto& cell {mGrid[row][column]};
        cell.mFirstSubCell.mWelds.mUpRight = false;
        cell.mSecondSubCell.mWelds.mUpRight = false;
    }
}

void Field::BreakLowerRightWeld(int row, int column) {
    if (column >= 0 && row < mNumRows) {
        auto& cell {mGrid[row][column]};
        cell.mFirstSubCell.mWelds.mDownRight = false;
        cell.mSecondSubCell.mWelds.mDownRight = false;
    }
}

Field::RemovedSubCells Field::RemoveRow(int rowIndex) {
    SetChanged();
    SaveState();
    
    RemovedSubCells removedSubCells;
    RemoveRowImpl(rowIndex, removedSubCells);
    
    return removedSubCells;
}

Field::RemovedSubCells Field::RemoveAreaOfSubCells(const Pht::IVec2& areaPos,
                                                   const Pht::IVec2& areaSize) {
    SetChanged();
    SaveState();
    
    RemovedSubCells removedSubCells;
    
    for (auto row {areaPos.y}; row < areaPos.y + areaSize.y; ++row) {
        for (auto column {areaPos.x}; column < areaPos.x + areaSize.x; ++column) {
            if (row < 0 || row >= mNumRows || column < 0 || column >= mNumColumns) {
                continue;
            }
            
            if (column == areaPos.x) {
                BreakCellRightWelds(row, column - 1);
            }

            if (column == areaPos.x + areaSize.x - 1) {
                BreakCellLeftWelds(row, column + 1);
            }

            if (row == areaPos.y) {
                BreakCellUpWelds(row - 1, column);
            }

            if (row == areaPos.y + areaSize.y - 1) {
                BreakCellDownWelds(row + 1, column);
            }
            
            auto& cell {mGrid[row][column]};
            ProcessSubCell(removedSubCells, cell.mFirstSubCell);
            ProcessSubCell(removedSubCells, cell.mSecondSubCell);
            
            cell = Cell {};
        }
    }
    
    BreakLowerLeftWeld(areaPos.y + areaSize.y, areaPos.x + areaSize.x);
    BreakUpperLeftWeld(areaPos.y - 1, areaPos.x + areaSize.x);
    BreakUpperRightWeld(areaPos.y - 1, areaPos.x - 1);
    BreakLowerRightWeld(areaPos.y + areaSize.y, areaPos.x - 1);
    
    return removedSubCells;
}

Field::RemovedSubCells Field::RemoveAllNonEmptySubCells() {
    SetChanged();
    RemovedSubCells removedSubCells;

    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            ProcessSubCell(removedSubCells, cell.mFirstSubCell);
            ProcessSubCell(removedSubCells, cell.mSecondSubCell);
            cell = Cell {};
        }
    }
    
    return removedSubCells;
}

void Field::RemoveWholePiece(int pieceId, Field::RemovedSubCells& removedSubCells) {
    for (auto row {0}; row < mNumRows; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            auto& cell {mGrid[row][column]};
            
            if (cell.IsEmpty()) {
                continue;
            }
            
            RemoveMatchingSubCell(pieceId, removedSubCells, cell.mFirstSubCell);
            RemoveMatchingSubCell(pieceId, removedSubCells, cell.mSecondSubCell);
        }
    }
}

void Field::RemoveMatchingSubCell(int pieceId,
                                  Field::RemovedSubCells& removedSubCells,
                                  SubCell& subCell) {
    if (subCell.mPieceId == pieceId) {
        SaveSubCellAndCancelFill(removedSubCells, subCell);
        
        subCell = SubCell {};
    }
}

void Field::ProcessSubCell(Field::RemovedSubCells& removedSubCells, const SubCell& subCell) {
    SaveSubCellAndCancelFill(removedSubCells, subCell);
    
    if (subCell.mIsPartOfIndivisiblePiece) {
        RemoveWholePiece(subCell.mPieceId, removedSubCells);
    }
}

void Field::SaveSubCellAndCancelFill(Field::RemovedSubCells& removedSubCells,
                                     const SubCell& subCell) {
    auto position {subCell.mPosition};
    
    if (subCell.mBlockRenderableKind != BlockRenderableKind::None) {
        RemovedSubCell removedSubCell {
            .mPosition = Pht::IVec2{
                static_cast<int>(position.x),
                static_cast<int>(position.y)
            },
            .mRotation = subCell.mRotation,
            .mRenderableKind = subCell.mBlockRenderableKind,
            .mColor = subCell.mColor,
            .mIsLevel = subCell.mIsLevel
        };
        
        removedSubCells.PushBack(removedSubCell);
    }
    
    if (mBlueprintGrid) {
        auto& blueprintSlotFillAnimation {
            (*mBlueprintGrid)[static_cast<int>(position.y)][static_cast<int>(position.x)].mAnimation
        };
        
        if (blueprintSlotFillAnimation.mIsActive) {
            blueprintSlotFillAnimation = BlueprintSlotFillAnimation {};
        }
    }
}

void Field::RemovePiece(int pieceId,
                        const Pht::IVec2& position,
                        int pieceNumRows,
                        int pieceNumColumns) {
    auto xBegin {position.x >= 0 ? position.x : 0};
    auto xEnd {position.x + pieceNumColumns};
    
    if (xEnd > mNumColumns) {
        xEnd = mNumColumns;
    }

    auto yBegin {position.y >= 0 ? position.y : 0};
    auto yEnd {position.y + pieceNumRows};
    
    if (yEnd >= mNumRows) {
        yEnd = mNumRows;
    }

    for (auto row {yBegin}; row < yEnd; ++row) {
        for (auto column {xBegin}; column < xEnd; ++column) {
            auto& cell {mGrid[row][column]};
            
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

FilledRowsResult Field::MarkFilledRowsAndCountPieceCellsInFilledRows(int pieceId) {
    FilledRowsResult result;
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto rowIndex {mLowestVisibleRow}; rowIndex < pastHighestVisibleRow; ++rowIndex) {
        auto& row {mGrid[rowIndex]};
        
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

void Field::UnmarkFilledRows(const FilledRowIndices& filledRowIndices) {
    for (auto i {0}; i < filledRowIndices.Size(); ++i) {
        auto& row {mGrid[filledRowIndices.At(i)]};

        for (auto& cell: row) {
            cell.mIsInFilledRow = false;
        }
    }
}