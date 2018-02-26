#include "FieldAnalyzer.hpp"

// Game includes.
#include "Field.hpp"

using namespace BlocksGame;

FieldAnalyzer::FieldAnalyzer(const Field& field) :
    mField {field} {}

float FieldAnalyzer::GetBurriedHolesAreaInVisibleRows() const {
    auto area {0.0f};
    auto numColumns {mField.GetNumColumns()};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    
    for (auto column {0}; column < numColumns; ++column) {
        auto aboveIsFilled {false};
        
        for (auto row {lowestVisibleRow + mField.GetNumRowsInOneScreen() - 1};
             row >= lowestVisibleRow;
             --row) {
            auto& cell {mField.mGrid[row][column]};
            
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

float FieldAnalyzer::GetWellsAreaInVisibleRows() const {
    auto area {0.0f};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    auto numColumns {mField.GetNumColumns()};
    
    for (auto column {0}; column < numColumns; ++column) {
        auto leftColumnIndex {column - 1};
        auto rightColumnIndex {column + 1};
        
        for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
            auto& cell {mField.mGrid[row][column]};
            
            if (!cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty {
                leftColumnIndex >= 0 ? mField.mGrid[row][leftColumnIndex].IsEmpty() : false
            };
            
            auto rightIsEmpty {
                rightColumnIndex < numColumns ? mField.mGrid[row][rightColumnIndex].IsEmpty() : false
            };
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
}

int FieldAnalyzer::GetNumTransitionsInVisibleRows() const {
    return GetNumTransitionsInColumns() + GetNumTransitionsInRows();
}

int FieldAnalyzer::GetNumTransitionsInColumns() const {
    auto numTransitions {0};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    auto numColumns {mField.GetNumColumns()};
    
    for (auto column {0}; column < numColumns; ++column) {
        for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow - 1; ++row) {
            // TODO: Exclude cells with mIsInFilledRow==true.
            auto thisCellIsEmpty {mField.mGrid[row][column].IsEmpty()};
            auto nextCellIsEmpty {mField.mGrid[row + 1][column].IsEmpty()};
            
            if (thisCellIsEmpty != nextCellIsEmpty) {
                ++numTransitions;
            }
        }
        
        auto bottomCellIsEmpty {mField.mGrid[lowestVisibleRow][column].IsEmpty()};
        
        if (bottomCellIsEmpty) {
            ++numTransitions;
        }
        
        auto topCellIsEmpty {mField.mGrid[pastHighestVisibleRow - 1][column].IsEmpty()};
        
        if (topCellIsEmpty) {
            ++numTransitions;
        }
    }
    
    return numTransitions;
}

int FieldAnalyzer::GetNumTransitionsInRows() const {
    auto numTransitions {0};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    auto numColumns {mField.GetNumColumns()};
    
    for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
        if (mField.mGrid[row][0].mIsInFilledRow) {
            continue;
        }
        
        for (auto column {0}; column < numColumns - 1; ++column) {
            auto thisCellIsEmpty {mField.mGrid[row][column].IsEmpty()};
            auto nextCellIsEmpty {mField.mGrid[row][column + 1].IsEmpty()};
            
            if (thisCellIsEmpty != nextCellIsEmpty) {
                ++numTransitions;
            }
        }
        
        auto leftCellIsEmpty {mField.mGrid[row][0].IsEmpty()};
        
        if (leftCellIsEmpty) {
            ++numTransitions;
        }
        
        auto rightCellIsEmpty {mField.mGrid[row][numColumns - 1].IsEmpty()};
        
        if (rightCellIsEmpty) {
            ++numTransitions;
        }
    }
    
    return numTransitions;
}

int FieldAnalyzer::GetNumCellsAccordingToBlueprintInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto result {0};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    auto numColumns {mField.GetNumColumns()};
    
    for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
        for (auto column {0}; column < numColumns; ++column) {
            if (mField.mGrid[row][column].mIsInFilledRow) {
                continue;
            }
            
            if (mField.IsCellAccordingToBlueprint(row, column)) {
                ++result;
            }
        }
    }

    return result;
}

float FieldAnalyzer::GetBuildHolesAreaInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto area {0.0f};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto numColumns {mField.GetNumColumns()};
    
    for (auto column {0}; column < numColumns; ++column) {
        auto aboveIsFilled {false};
        
        for (auto row {lowestVisibleRow + mField.GetNumRowsInOneScreen() - 1};
             row >= lowestVisibleRow;
             --row) {
            auto& cell {mField.mGrid[row][column]};
            
            if (cell.mIsInFilledRow) {
                continue;
            }
            
            auto thisCellShouldBeFilled {
                (*mField.mBlueprintGrid)[row][column].mFill != Fill::Empty
            };
            
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

float FieldAnalyzer::GetBuildWellsAreaInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto area {0.0f};
    auto lowestVisibleRow {mField.GetLowestVisibleRow()};
    auto numColumns {mField.GetNumColumns()};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    
    for (auto column {0}; column < numColumns; ++column) {
        auto leftColumnIndex {column - 1};
        auto rightColumnIndex {column + 1};
        
        for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; ++row) {
            auto& cell {mField.mGrid[row][column]};
            
            auto thisCellShouldBeFilled {
                (*mField.mBlueprintGrid)[row][column].mFill != Fill::Empty
            };
            
            if (!thisCellShouldBeFilled || !cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty {
                leftColumnIndex >= 0 ? mField.mGrid[row][leftColumnIndex].IsEmpty() : false
            };
            
            auto rightIsEmpty {
                rightColumnIndex < numColumns ? mField.mGrid[row][rightColumnIndex].IsEmpty() : false
            };
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
}
