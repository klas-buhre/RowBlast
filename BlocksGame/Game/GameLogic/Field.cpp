#include "Field.hpp"

#include <assert.h>

// Game includes.
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "Level.hpp"
#include "CollisionDetection.hpp"

using namespace BlocksGame;

namespace {
    const auto maxNumScanTries {Field::maxNumRows};
    const auto maxNumRowsInOneScreen {19};

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
    }
    
    void BreakRightWelds(Welds& welds) {
        welds.mUpRight = false;
        welds.mRight = false;
        welds.mDownRight = false;
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
    
    int XMin(const PieceBlockCoords& pieceBlockCoords) {
        auto xMin {Field::maxNumColumns - 1};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto x {pieceBlockCoords.At(i).x};
            
            if (x < xMin) {
                xMin = x;
            }
        }
        
        return xMin;
    }
    
    int XMax(const PieceBlockCoords& pieceBlockCoords) {
        auto xMax {0};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto x {pieceBlockCoords.At(i).x};
            
            if (x > xMax) {
                xMax = x;
            }
        }
        
        return xMax;
    }

    int YMin(const PieceBlockCoords& pieceBlockCoords) {
        auto yMin {10000};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto y {pieceBlockCoords.At(i).y};
            
            if (y < yMin) {
                yMin = y;
            }
        }
        
        return yMin;
    }
    
    int YMax(const PieceBlockCoords& pieceBlockCoords) {
        auto yMax {0};
        
        for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
            auto y {pieceBlockCoords.At(i).y};
            
            if (y > yMax) {
                yMax = y;
            }
        }
        
        return yMax;
    }
}

Field::Field() {
    std::vector<Cell> emptyRow(Piece::maxColumns);
    
    for (auto i {0}; i < Piece::maxRows; ++i) {
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

int Field::GetNumCellsAccordingToBlueprintInVisibleRows() const {
    assert(mBlueprintGrid);
    
    auto result {0};
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto row {mLowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
        for (auto column {0}; column < mNumColumns; ++column) {
            if (mGrid[row][column].mIsInFilledRow) {
                continue;
            }
            
            if (IsCellAccordingToBlueprint(row, column)) {
                ++result;
            }
        }
    }

    return result;
}

float Field::GetBuildHolesAreaInVisibleRows() const {
    assert(mBlueprintGrid);
    
    auto area {0.0f};
    
    for (auto column {0}; column < mNumColumns; ++column) {
        auto aboveIsFilled {false};
        
        for (auto row {mLowestVisibleRow + GetNumRowsInOneScreen() - 1};
             row >= mLowestVisibleRow;
             --row) {
            auto& cell {mGrid[row][column]};
            
            if (cell.mIsInFilledRow) {
                continue;
            }
            
            auto thisCellShouldBeFilled {(*mBlueprintGrid)[row][column].mFill != Fill::Empty};
            
            if (cell.IsEmpty()) {
                if (aboveIsFilled && thisCellShouldBeFilled) {
                    area += 1.0f;
                }
                
                continue;
            }
            
            if (cell.IsFull()) {
                aboveIsFilled = true;
                continue;
            }
            
            auto cellFill {
                cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill
            };
            
            switch (cellFill) {
                case Fill::UpperLeftHalf:
                case Fill::UpperRightHalf:
                    if (thisCellShouldBeFilled) {
                        area += 1.0f;
                    }
                    break;
                case Fill::LowerLeftHalf:
                case Fill::LowerRightHalf:
                    if (aboveIsFilled && thisCellShouldBeFilled) {
                        area += 1.0f;
                    }
                    break;
                case Fill::Empty:
                case Fill::Full:
                    break;
            }
            
            aboveIsFilled = true;
        }
    }
    
    return area;
}

float Field::GetBuildWellsAreaInVisibleRows() const {
    assert(mBlueprintGrid);
    
    auto area {0.0f};
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto column {0}; column < mNumColumns; ++column) {
        auto leftColumnIndex {column - 1};
        auto rightColumnIndex {column + 1};
        
        for (auto row {mLowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
            auto& cell {mGrid[row][column]};
            auto thisCellShouldBeFilled {(*mBlueprintGrid)[row][column].mFill != Fill::Empty};
            
            if (!thisCellShouldBeFilled || !cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty {
                leftColumnIndex >= 0 ? mGrid[row][leftColumnIndex].IsEmpty() : false
            };
            
            auto rightIsEmpty {
                rightColumnIndex < mNumColumns ? mGrid[row][rightColumnIndex].IsEmpty() : false
            };
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
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

float Field::GetBurriedHolesAreaInVisibleRows() const {
    auto area {0.0f};
    
    for (auto column {0}; column < mNumColumns; ++column) {
        auto aboveIsFilled {false};
        
        for (auto row {mLowestVisibleRow + GetNumRowsInOneScreen() - 1};
             row >= mLowestVisibleRow;
             --row) {
            auto& cell {mGrid[row][column]};
            
            if (cell.mIsInFilledRow) {
                continue;
            }
            
            if (cell.IsEmpty()) {
                if (aboveIsFilled) {
                    area += 1.0f;
                }
                
                continue;
            }
            
            if (cell.IsFull()) {
                aboveIsFilled = true;
                continue;
            }
            
            auto fill {
                cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill
            };
            
            switch (fill) {
                case Fill::UpperLeftHalf:
                case Fill::UpperRightHalf:
                    area += 1.0f;
                    break;
                case Fill::LowerLeftHalf:
                case Fill::LowerRightHalf:
                    if (aboveIsFilled) {
                        area += 1.0f;
                    }
                    break;
                case Fill::Empty:
                case Fill::Full:
                    break;
            }
            
            aboveIsFilled = true;
        }
    }
    
    return area;
}

float Field::GetWellsAreaInVisibleRows() const {
    auto area {0.0f};
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto column {0}; column < mNumColumns; ++column) {
        auto leftColumnIndex {column - 1};
        auto rightColumnIndex {column + 1};
        
        for (auto row {mLowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
            auto& cell {mGrid[row][column]};
            
            if (!cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty {
                leftColumnIndex >= 0 ? mGrid[row][leftColumnIndex].IsEmpty() : false
            };
            
            auto rightIsEmpty {
                rightColumnIndex < mNumColumns ? mGrid[row][rightColumnIndex].IsEmpty() : false
            };
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
}

int Field::GetNumTransitionsInVisibleRows() const {
    return GetNumTransitionsInColumns() + GetNumTransitionsInRows();
}

int Field::GetNumTransitionsInColumns() const {
    auto numTransitions {0};
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto column {0}; column < mNumColumns; ++column) {
        for (auto row {mLowestVisibleRow}; row < pastHighestVisibleRow - 1; ++row) {
            // TODO: Exclude cells with mIsInFilledRow==true.
            auto thisCellIsEmpty {mGrid[row][column].IsEmpty()};
            auto nextCellIsEmpty {mGrid[row + 1][column].IsEmpty()};
            
            if (thisCellIsEmpty != nextCellIsEmpty) {
                ++numTransitions;
            }
        }
        
        auto bottomCellIsEmpty {mGrid[mLowestVisibleRow][column].IsEmpty()};
        
        if (bottomCellIsEmpty) {
            ++numTransitions;
        }
        
        auto topCellIsEmpty {mGrid[pastHighestVisibleRow - 1][column].IsEmpty()};
        
        if (topCellIsEmpty) {
            ++numTransitions;
        }
    }
    
    return numTransitions;
}

int Field::GetNumTransitionsInRows() const {
    auto numTransitions {0};
    auto pastHighestVisibleRow {mLowestVisibleRow + GetNumRowsInOneScreen()};
    
    for (auto row {mLowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
        if (mGrid[row][0].mIsInFilledRow) {
            continue;
        }
        
        for (auto column {0}; column < mNumColumns - 1; ++column) {
            auto thisCellIsEmpty {mGrid[row][column].IsEmpty()};
            auto nextCellIsEmpty {mGrid[row][column + 1].IsEmpty()};
            
            if (thisCellIsEmpty != nextCellIsEmpty) {
                ++numTransitions;
            }
        }
        
        auto leftCellIsEmpty {mGrid[row][0].IsEmpty()};
        
        if (leftCellIsEmpty) {
            ++numTransitions;
        }
        
        auto rightCellIsEmpty {mGrid[row][mNumColumns - 1].IsEmpty()};
        
        if (rightCellIsEmpty) {
            ++numTransitions;
        }
    }
    
    return numTransitions;
}

const Cell& Field::GetCell(int row, int column) const {
    assert(row >= 0 && column >= 0 && row < mNumRows && column < mNumColumns);
    return mGrid[row][column];
}

Cell& Field::GetCell(int row, int column) {
    assert(row >= 0 && column >= 0 && row < mNumRows && column < mNumColumns);
    return mGrid[row][column];
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
    CollisionResult collisionResult;
    
    for (;;) {
        CheckCollision(collisionResult, pieceBlocks, position, step, isScanStart);
        
        if (isScanStart) {
            isScanStart = false;
        }
        
        if (collisionResult.mIsCollision == IsCollision::Yes) {
            return position;
        }
        
        if (collisionResult.mIsCollision == IsCollision::NextWillBe) {
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
    auto pieceNumRows {pieceBlocks.mNumRows};
    auto pieceNumColumns {pieceBlocks.mNumColumns};
    auto& pieceGrid {pieceBlocks.mGrid};

    for (auto pieceRow {0}; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto& pieceSubCell {pieceGrid[pieceRow][pieceColumn].mFirstSubCell};
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
        CollisionResult collisionResult;
        CheckCollision(collisionResult, pieceBlocks, position, step, isScanStart);
        
        if (collisionResult.mIsCollision != IsCollision::Yes) {
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

CollisionPoints Field::GetOccupiedArea(const PieceBlocks& pieceBlocks,
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
    
    auto pieceId {subCell.mPieceId};
    Pht::IVec2 piecePosition {0, 0};
    auto pieceBlocks {ExtractPieceBlocks(piecePosition, subCell, scanPosition, scanDirection)};

    Pht::IVec2 step {0, -1};
    auto collisionPosition {ScanUntilCollision(pieceBlocks, piecePosition, step)};
    
    if (collisionPosition.y + 1 >= mLowestVisibleRow) {
        LandPieceBlocks(pieceBlocks, pieceId, collisionPosition + Pht::IVec2 {0, 1}, false, false);
    } else {
        LandPieceBlocks(pieceBlocks, pieceId, piecePosition, false, false);
    }
}

PieceBlocks Field::ExtractPieceBlocks(Pht::IVec2& piecePosition,
                                      const SubCell& subCell,
                                      const Pht::IVec2& scanPosition,
                                      ScanDirection scanDirection) {
    PieceBlockCoords pieceBlockCoords;
    auto pieceId {subCell.mPieceId};
    
    FindPieceBlocks(pieceBlockCoords, pieceId, scanPosition);
    
    piecePosition.x = XMin(pieceBlockCoords);
    piecePosition.y = YMin(pieceBlockCoords);

    ClearPieceBlockGrid();
    
    for (auto i {0}; i < pieceBlockCoords.Size(); ++i) {
        auto& coord {pieceBlockCoords.At(i)};
        auto pieceRow {coord.y - piecePosition.y};
        auto pieceColumn {coord.x - piecePosition.x};
        auto& subCell {GetSubCell(coord, pieceId)};
        subCell.mIsFound = false;
        subCell.mTriedScanDirection = scanDirection;
        
        mPieceBlockGrid[pieceRow][pieceColumn].mFirstSubCell = subCell;
        subCell = SubCell {};
    }

    return PieceBlocks {
        mPieceBlockGrid,
        YMax(pieceBlockCoords) - piecePosition.y + 1,
        XMax(pieceBlockCoords) - piecePosition.x + 1
    };
}

void Field::FindPieceBlocks(PieceBlockCoords& pieceBlockCoords,
                            int pieceId,
                            const Pht::IVec2& position) {
    auto& subCell {GetSubCell(position, pieceId)};
    
    if (subCell.mIsFound) {
        return;
    }
    
    pieceBlockCoords.PushBack(position);
    subCell.mIsFound = true;
    auto& welds {subCell.mWelds};
    
    if (welds.mUp) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {0, 1});
    }

    if (welds.mUpRight) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {1, 1});
    }
    
    if (welds.mRight) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {1, 0});
    }
    
    if (welds.mDownRight) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {1, -1});
    }

    if (welds.mDown) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {0, -1});
    }

    if (welds.mDownLeft) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {-1, -1});
    }

    if (welds.mLeft) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {-1, 0});
    }

    if (welds.mUpLeft) {
        FindPieceBlocks(pieceBlockCoords, pieceId, position + Pht::IVec2 {-1, 1});
    }
}

SubCell& Field::GetSubCell(const Pht::IVec2& position, int pieceId) {
    auto& cell {mGrid[position.y][position.x]};
    
    if (cell.mFirstSubCell.mPieceId == pieceId) {
        return cell.mFirstSubCell;
    } else {
        return cell.mSecondSubCell;
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
    for (auto row {0}; row < Piece::maxRows; ++row) {
        for (auto column {0}; column < Piece::maxColumns; ++column) {
            mPieceBlockGrid[row][column] = Cell {};
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
    
    if (subCell.mRenderableObject) {
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
