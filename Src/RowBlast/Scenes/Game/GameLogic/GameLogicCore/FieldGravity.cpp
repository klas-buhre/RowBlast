#include "FieldGravity.hpp"

using namespace RowBlast;

namespace {
    int XMin(const FieldGravity::PieceBlockCoords& pieceBlockCoords) {
        auto xMin = Field::maxNumColumns - 1;
        
        for (auto i = 0; i < pieceBlockCoords.Size(); ++i) {
            auto x = pieceBlockCoords.At(i).mPosition.x;
            if (x < xMin) {
                xMin = x;
            }
        }
        
        return xMin;
    }
    
    int XMax(const FieldGravity::PieceBlockCoords& pieceBlockCoords) {
        auto xMax = 0;
        
        for (auto i = 0; i < pieceBlockCoords.Size(); ++i) {
            auto x = pieceBlockCoords.At(i).mPosition.x;
            if (x > xMax) {
                xMax = x;
            }
        }
        
        return xMax;
    }

    int YMin(const FieldGravity::PieceBlockCoords& pieceBlockCoords) {
        auto yMin = 10000;
        
        for (auto i = 0; i < pieceBlockCoords.Size(); ++i) {
            auto y = pieceBlockCoords.At(i).mPosition.y;
            if (y < yMin) {
                yMin = y;
            }
        }
        
        return yMin;
    }
    
    int YMax(const FieldGravity::PieceBlockCoords& pieceBlockCoords) {
        auto yMax = 0;
        
        for (auto i = 0; i < pieceBlockCoords.Size(); ++i) {
            auto y = pieceBlockCoords.At(i).mPosition.y;
            if (y > yMax) {
                yMax = y;
            }
        }
        
        return yMax;
    }
}

FieldGravity::FieldGravity(Field& field) :
    mField {field} {}

void FieldGravity::Init() {
    mPieceBlockGrid.clear();
    
    std::vector<Cell> emptyRow(mField.GetNumColumns());
    
    for (auto i = 0; i < mField.GetNumRows(); ++i) {
        mPieceBlockGrid.push_back(emptyRow);
    }
    
    mNumDirtyPieceBlockGridRows = 0;
}

void FieldGravity::PullDownLoosePieces() {
    mField.SetChanged();
    mAnyPiecesPulledDown = false;
    
    for (auto row = mField.mLowestVisibleRow; row < mField.mNumRows; ++row) {
        for (auto column = 0; column < mField.mNumColumns; ++column) {
            PullDownPiece(row, column, ScanDirection::Right);
        }
        
        for (auto column = mField.mNumColumns - 1; column >= 0; --column) {
            PullDownPiece(row, column, ScanDirection::Left);
        }
    }
    
    ResetAllCellsTriedScanDirection();
}

void FieldGravity::PullDownPiece(int row, int column, ScanDirection scanDirection) {
    auto& cell = mField.mGrid[row][column];
    Pht::IVec2 position {column, row};
    
    PullDownPiece(cell.mFirstSubCell, position, scanDirection);
    PullDownPiece(cell.mSecondSubCell, position, scanDirection);
}

void FieldGravity::PullDownPiece(const SubCell& subCell,
                                 const Pht::IVec2& scanPosition,
                                 ScanDirection scanDirection) {
    if (subCell.IsEmpty() || subCell.mIsGrayLevelBlock ||
        subCell.mTriedScanDirection == scanDirection) {
        
        return;
    }
    
    Pht::IVec2 piecePosition {0, 0};
    auto pieceBlocks =
        ExtractPieceBlocks(piecePosition, subCell.mColor, scanPosition, scanDirection);

    Pht::IVec2 step {0, -1};
    auto collisionPosition = mField.ScanUntilCollision(pieceBlocks, piecePosition, step);
    if (collisionPosition.y >= piecePosition.y) {
        LandPulledDownPieceBlocks(pieceBlocks, piecePosition);
    } else {
        LandPulledDownPieceBlocks(pieceBlocks, collisionPosition + Pht::IVec2 {0, 1});
    }
    
    if (piecePosition.y - collisionPosition.y > 1) {
        mAnyPiecesPulledDown = true;
    }
}

PieceBlocks FieldGravity::ExtractPieceBlocks(Pht::IVec2& piecePosition,
                                             BlockColor color,
                                             const Pht::IVec2& scanPosition,
                                             ScanDirection scanDirection) {
    mPieceBlockCoords.Clear();
    FindPieceClusterBlocks(color, scanPosition);

    piecePosition.x = XMin(mPieceBlockCoords);
    piecePosition.y = YMin(mPieceBlockCoords);

    ClearPieceBlockGrid();
    
    auto pieceRowMax = 0;
    
    for (auto i = 0; i < mPieceBlockCoords.Size(); ++i) {
        auto& coord = mPieceBlockCoords.At(i);
        auto& position = coord.mPosition;
        auto pieceRow = position.y - piecePosition.y;
        auto pieceColumn = position.x - piecePosition.x;
        
        if (pieceRow > pieceRowMax) {
            pieceRowMax = pieceRow;
        }
        
        auto& cell = mField.mGrid[position.y][position.x];
        auto& fieldSubCell = coord.mIsFirstSubCell ? cell.mFirstSubCell : cell.mSecondSubCell;
        
        fieldSubCell.mIsFound = false;
        fieldSubCell.mTriedScanDirection = scanDirection;
        
        auto& pieceBlockCell = mPieceBlockGrid[pieceRow][pieceColumn];
        if (pieceBlockCell.mFirstSubCell.IsEmpty()) {
            pieceBlockCell.mFirstSubCell = fieldSubCell;
        } else {
            pieceBlockCell.mSecondSubCell = fieldSubCell;
        }
        
        fieldSubCell = SubCell {};
    }
    
    mNumDirtyPieceBlockGridRows = pieceRowMax + 1;

    return PieceBlocks {
        mPieceBlockGrid,
        YMax(mPieceBlockCoords) - piecePosition.y + 1,
        XMax(mPieceBlockCoords) - piecePosition.x + 1
    };
}

void FieldGravity::FindPieceClusterBlocks(BlockColor color, const Pht::IVec2& position) {
    auto& firstSubCell = mField.mGrid[position.y][position.x].mFirstSubCell;
    if (firstSubCell.IsNonBlockObject()) {
        if (firstSubCell.IsBigAsteroid()) {
            FindAsteroidCells(position);
        } else {
            firstSubCell.mIsFound = true;
            PieceBlockCoord pieceCoord {position, true};
            mPieceBlockCoords.PushBack(pieceCoord);
        }
        
        return;
    }
    
    FindPieceBlocks(color, position);
    
    auto xMin = XMin(mPieceBlockCoords);
    auto yMin = YMin(mPieceBlockCoords);
    auto xMax = XMax(mPieceBlockCoords);
    auto yMax = YMax(mPieceBlockCoords);
    
    enum class ColumnScanState {
        Idle,
        InsideFoundPiece,
        AboveFoundPiece
    };
    
    for (auto column = xMin; column <= xMax; ++column) {
        auto state = ColumnScanState::Idle;
        auto aboveFoundPieceRow = 0;

        for (auto row = yMin; row <= yMax; ++row) {
            auto& cell = mField.mGrid[row][column];
            auto& firstSubCell = cell.mFirstSubCell;
            auto& secondSubCell = cell.mSecondSubCell;
            
            switch (state) {
                case ColumnScanState::Idle:
                    if (firstSubCell.mIsFound || secondSubCell.mIsFound) {
                        state = ColumnScanState::InsideFoundPiece;
                    }
                    break;
                case ColumnScanState::InsideFoundPiece:
                    if ((!firstSubCell.mIsFound && !firstSubCell.IsEmpty()) ||
                        (!secondSubCell.mIsFound && !secondSubCell.IsEmpty())) {
                        state = ColumnScanState::AboveFoundPiece;
                        aboveFoundPieceRow = row;
                    }
                    break;
                case ColumnScanState::AboveFoundPiece:
                    if (firstSubCell.mIsFound || secondSubCell.mIsFound) {
                        // Found a concave part of the found piece. Need to find any blocks inside
                        // that concave area and add those to the found piece coords to prevent the
                        // cluster from getting stuck when pulled down by gravity.
                        // TODO: this algorithm does not completely work with triangular blocks or
                        // the case when any piece is locked in two concave areas in two other
                        // pieces.
                        state = ColumnScanState::InsideFoundPiece;
                        for (auto undiscoveredCellRow = aboveFoundPieceRow;
                             undiscoveredCellRow < row;
                             ++undiscoveredCellRow) {
                            auto& undiscoveredCell = mField.mGrid[undiscoveredCellRow][column];
                            auto& firstUndiscoveredSubCell = undiscoveredCell.mFirstSubCell;
                            auto& secondUndiscoveredSubCell = undiscoveredCell.mSecondSubCell;
                            Pht::IVec2 undiscoveredCellPosition {column, undiscoveredCellRow};
                            if (!firstUndiscoveredSubCell.IsEmpty() &&
                                !firstUndiscoveredSubCell.mIsFound) {
                                FindPieceClusterBlocks(firstUndiscoveredSubCell.mColor,
                                                       undiscoveredCellPosition);
                            }
                            if (!secondUndiscoveredSubCell.IsEmpty() &&
                                !secondUndiscoveredSubCell.mIsFound) {
                                FindPieceClusterBlocks(firstUndiscoveredSubCell.mColor,
                                                       undiscoveredCellPosition);
                            }
                        }
                    }
                    break;
            }
        }
    }
}

void FieldGravity::FindPieceBlocks(BlockColor color, const Pht::IVec2& position) {
    auto& cell = mField.mGrid[position.y][position.x];
    auto& firstSubCell = cell.mFirstSubCell;
    auto& secondSubCell = cell.mSecondSubCell;
    SubCell* subCell {nullptr};
    auto isFirstSubCell = true;
    
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
    auto& welds = subCell->mWelds;

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

void FieldGravity::FindAsteroidCells(const Pht::IVec2& position) {
    if (position.y < 0 || position.y >= mField.mNumRows || position.x < 0 ||
        position.x >= mField.mNumColumns) {
        
        return;
    }
    
    auto& subCell = mField.mGrid[position.y][position.x].mFirstSubCell;
    if (!subCell.IsBigAsteroid() || subCell.mIsFound) {
        return;
    }

    PieceBlockCoord coord {position, true};
    mPieceBlockCoords.PushBack(coord);
    
    subCell.mIsFound = true;
    
    FindAsteroidCells(position + Pht::IVec2 {0, 1});
    FindAsteroidCells(position + Pht::IVec2 {1, 0});
    FindAsteroidCells(position + Pht::IVec2 {0, -1});
    FindAsteroidCells(position + Pht::IVec2 {-1, 0});
}

void FieldGravity::ResetAllCellsTriedScanDirection() {
    for (auto row = 0; row < mField.mNumRows; ++row) {
        for (auto column = 0; column < mField.mNumColumns; ++column) {
            auto& cell = mField.mGrid[row][column];
            cell.mFirstSubCell.mTriedScanDirection = ScanDirection::None;
            cell.mSecondSubCell.mTriedScanDirection = ScanDirection::None;
        }
    }
}

void FieldGravity::ClearPieceBlockGrid() {
    for (auto row = 0; row < mNumDirtyPieceBlockGridRows; ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            mPieceBlockGrid[row][column] = Cell {};
        }
    }
}

void FieldGravity::LandPulledDownPieceBlocks(const PieceBlocks& pieceBlocks,
                                             const Pht::IVec2& position) {
    for (auto pieceRow = 0; pieceRow < pieceBlocks.mNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceBlocks.mNumColumns; ++pieceColumn) {
            auto& pieceCell = pieceBlocks.mGrid[pieceRow][pieceColumn];
            if (pieceCell.IsEmpty()) {
                continue;
            }
            
            auto row = position.y + pieceRow;
            auto column = position.x + pieceColumn;
            auto& fieldCell = mField.mGrid[row][column];
            
            if (pieceCell.mSecondSubCell.IsEmpty()) {
                auto& fieldSubCell =
                    fieldCell.mFirstSubCell.IsEmpty() ?
                    fieldCell.mFirstSubCell : fieldCell.mSecondSubCell;
                
                fieldSubCell = pieceCell.mFirstSubCell;
            } else {
                fieldCell = pieceCell;
            }
        }
    }
}

void FieldGravity::ShiftFieldDown(int rowIndex) {
    for (;;) {
        PullDownLoosePieces();
        auto anyBlocksShiftedDown = ShiftGrayBlocksDown(rowIndex);
    
        // Need to do the same procedure again so that any blocks that were stuck the previous pass
        // can be moved in the following attempt.
        if (!mAnyPiecesPulledDown && !anyBlocksShiftedDown) {
            break;
        }
    }

    ResetAllCellsShiftedDownFlag();
}

bool FieldGravity::ShiftGrayBlocksDown(int rowIndex) {
    auto anyBlocksShiftedDown = false;
    
    for (auto column = 0; column < mField.mNumColumns; ++column) {
        for (auto row = rowIndex; row < mField.mNumRows - 1; ++row) {
            auto& lowerCell = mField.mGrid[row][column];
            auto& upperCell = mField.mGrid[row + 1][column];

            if (upperCell.mFirstSubCell.mIsGrayLevelBlock && !upperCell.mIsShiftedDown &&
                lowerCell.IsEmpty()) {
                
                lowerCell = upperCell;
                lowerCell.mIsShiftedDown = true;
                upperCell = Cell {};
                anyBlocksShiftedDown = true;
            }
        }
    }
    
    return anyBlocksShiftedDown;
}

void FieldGravity::ResetAllCellsShiftedDownFlag() {
    for (auto row = 0; row < mField.mNumRows; ++row) {
        for (auto column = 0; column < mField.mNumColumns; ++column) {
            mField.mGrid[row][column].mIsShiftedDown = false;
        }
    }
}

void FieldGravity::DetectBlocksThatShouldNotBounce() {
    for (auto row = mField.mNumRows - 1; row >= mField.mLowestVisibleRow; --row) {
        for (auto column = 0; column < mField.mNumColumns; ++column) {
            auto& subCell = mField.mGrid[row][column].mFirstSubCell;
            if (subCell.mIsGrayLevelBlock && subCell.mFallingBlockAnimation.mShouldBounce &&
                subCell.mTriedScanDirection == ScanDirection::None && subCell.mPosition.y > row) {
                
                Pht::IVec2 gridPosition {column, row};
                if (IsBlockStructureFloating(gridPosition) == IsFloating::Yes) {
                    SetShouldNotBounce(gridPosition);
                }
                
                SetIsScanned(gridPosition);
            }
        }
    }
    
    ResetAllCellsFoundFlag();
    ResetAllCellsTriedScanDirection();
}

FieldGravity::IsFloating FieldGravity::IsBlockStructureFloating(const Pht::IVec2& gridPosition) {
    if (IsOutsideVisibleField(gridPosition)) {
        return IsFloating::Unknown;
    }
    
    auto& subCell = mField.mGrid[gridPosition.y][gridPosition.x].mFirstSubCell;
    if (!subCell.mIsGrayLevelBlock || !subCell.mFallingBlockAnimation.mShouldBounce ||
        subCell.mIsFound || subCell.mTriedScanDirection != ScanDirection::None) {
        
        return IsFloating::Unknown;
    }
    
    subCell.mIsFound = true;
    
    if (gridPosition.y == mField.mLowestVisibleRow) {
        return IsFloating::No;
    }
    
    if (gridPosition.y - 1 >= mField.mLowestVisibleRow) {
        auto& lowerCell = mField.mGrid[gridPosition.y - 1][gridPosition.x];
        if (!lowerCell.IsEmpty()) {
            auto& firstLowerSubCell = lowerCell.mFirstSubCell;
            if (subCell.mPosition.y - firstLowerSubCell.mPosition.y >= 2.0f) {
                return IsFloating::No;
            }
            
            if (!firstLowerSubCell.IsEmpty() && !firstLowerSubCell.mIsGrayLevelBlock) {
                return IsFloating::No;
            }
            
            auto& secondLowerSubCell = lowerCell.mSecondSubCell;
            if (!secondLowerSubCell.IsEmpty() && !secondLowerSubCell.mIsGrayLevelBlock) {
                return IsFloating::No;
            }
        }
    }
    
    if (IsBlockStructureFloating(gridPosition + Pht::IVec2 {0, -1}) == IsFloating::No) {
        return IsFloating::No;
    }
    
    if (IsBlockStructureFloating(gridPosition + Pht::IVec2 {1, 0}) == IsFloating::No) {
        return IsFloating::No;
    }

    if (IsBlockStructureFloating(gridPosition + Pht::IVec2 {0, 1}) == IsFloating::No) {
        return IsFloating::No;
    }

    if (IsBlockStructureFloating(gridPosition + Pht::IVec2 {-1, 0}) == IsFloating::No) {
        return IsFloating::No;
    }

    return IsFloating::Yes;
}

bool FieldGravity::IsOutsideVisibleField(const Pht::IVec2& gridPosition) {
    if (gridPosition.y < mField.mLowestVisibleRow || gridPosition.y >= mField.mNumRows ||
        gridPosition.x < 0 || gridPosition.x >= mField.mNumColumns) {
        
        return true;
    }
    
    return false;
}

void FieldGravity::SetShouldNotBounce(const Pht::IVec2& gridPosition) {
    if (IsOutsideVisibleField(gridPosition)) {
        return;
    }
    
    auto& subCell = mField.mGrid[gridPosition.y][gridPosition.x].mFirstSubCell;
    if (!subCell.mIsGrayLevelBlock || !subCell.mFallingBlockAnimation.mShouldBounce ||
        subCell.mTriedScanDirection != ScanDirection::None) {
        
        return;
    }
    
    subCell.mFallingBlockAnimation.mShouldBounce = false;
    
    SetShouldNotBounce(gridPosition + Pht::IVec2 {0, 1});
    SetShouldNotBounce(gridPosition + Pht::IVec2 {1, 0});
    SetShouldNotBounce(gridPosition + Pht::IVec2 {0, -1});
    SetShouldNotBounce(gridPosition + Pht::IVec2 {-1, 0});
}

void FieldGravity::SetIsScanned(const Pht::IVec2& gridPosition) {
    if (IsOutsideVisibleField(gridPosition)) {
        return;
    }
    
    auto& subCell = mField.mGrid[gridPosition.y][gridPosition.x].mFirstSubCell;
    if (!subCell.mIsGrayLevelBlock || subCell.mTriedScanDirection != ScanDirection::None) {
        return;
    }
    
    subCell.mTriedScanDirection = ScanDirection::Right;
    
    SetIsScanned(gridPosition + Pht::IVec2 {-1, 0});
    SetIsScanned(gridPosition + Pht::IVec2 {0, 1});
    SetIsScanned(gridPosition + Pht::IVec2 {1, 0});
    
    if (gridPosition.y - 1 >= mField.mLowestVisibleRow) {
        auto& lowerCell = mField.mGrid[gridPosition.y - 1][gridPosition.x];
        if (!lowerCell.IsEmpty() &&
            subCell.mPosition.y - lowerCell.mFirstSubCell.mPosition.y >= 2.0f) {
            
            return;
        }
    }

    SetIsScanned(gridPosition + Pht::IVec2 {0, -1});
}

void FieldGravity::ResetAllCellsFoundFlag() {
    for (auto row = 0; row < mField.mNumRows; ++row) {
        for (auto column = 0; column < mField.mNumColumns; ++column) {
            auto& cell = mField.mGrid[row][column];
            cell.mFirstSubCell.mIsFound = false;
            cell.mSecondSubCell.mIsFound = false;
        }
    }
}
