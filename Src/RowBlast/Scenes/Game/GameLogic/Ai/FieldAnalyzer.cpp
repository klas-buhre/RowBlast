#include "FieldAnalyzer.hpp"

#include <assert.h>

// Game includes.
#include "Field.hpp"

using namespace RowBlast;

namespace {
    enum class HoleScanState {
        Idle,
        InsideFilledRow,
        InsideLandedPiece,
        LandedPieceSitsOnOrHangsUnderFilledRow,
        AboveIsFilled
    };

    struct BurriedHoleScanStates {
        HoleScanState mState {HoleScanState::Idle};
        HoleScanState mPreviousState {HoleScanState::Idle};
    };
    
    BurriedHoleScanStates TransitionHoleState(HoleScanState newState,
                                              BurriedHoleScanStates states) {
        states.mPreviousState = states.mState;
        states.mState = newState;
        return states;
    }

    float CalculateAreaContribution(const Cell& cell) {
        if (cell.IsEmpty()) {
            return 1.0f;
        }
        
        if (cell.IsFull()) {
            return 0.0f;
        }
        
        auto fill =
            cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill;
    
        switch (fill) {
            case Fill::UpperLeftHalf:
            case Fill::UpperRightHalf:
            case Fill::LowerLeftHalf:
            case Fill::LowerRightHalf:
                return 1.0f;
            case Fill::Empty:
            case Fill::Full:
                assert(false);
                return 0.0f;
        }
    }

    float CalculateLowerHalfCellAreaContribution(const Cell& cell) {
        if (cell.IsEmpty() || cell.IsFull()) {
            return 0.0f;
        }
        
        auto fill =
            cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill;
        
        switch (fill) {
            case Fill::UpperLeftHalf:
            case Fill::UpperRightHalf:
                return 1.0f;
            default:
                return 0.0f;
        }
    }

    float CalculateAreaContributionUnderFilledRow(const Cell& cell, HoleScanState previousState) {
        switch (previousState) {
            case HoleScanState::Idle:
                return CalculateLowerHalfCellAreaContribution(cell);
            case HoleScanState::InsideFilledRow:
            case HoleScanState::InsideLandedPiece:
            case HoleScanState::LandedPieceSitsOnOrHangsUnderFilledRow:
                assert(false);
                return 0.0f;
            case HoleScanState::AboveIsFilled:
                return CalculateAreaContribution(cell);
        }
    }
    
    bool IsCellPartOfLandedPiece(const Cell& cell, int landedPieceId) {
        return cell.mFirstSubCell.mPieceId == landedPieceId ||
               cell.mSecondSubCell.mPieceId == landedPieceId;
    }
}

FieldAnalyzer::FieldAnalyzer(const Field& field) :
    mField {field} {}

float FieldAnalyzer::CalculateBurriedHolesAreaInVisibleRows() const {
    auto area = 0.0f;
    auto numColumns = mField.GetNumColumns();
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    
    for (auto column = 0; column < numColumns; ++column) {
        auto aboveIsFilled = false;
        
        for (auto row = lowestVisibleRow + mField.GetNumRowsInOneScreen() - 1;
             row >= lowestVisibleRow;
             --row) {
            auto& cell = mField.mGrid[row][column];
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
            
            auto fill =
                cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill;
            
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
                    assert(false);
                    break;
            }
            
            aboveIsFilled = true;
        }
    }
    
    return area;
}

float FieldAnalyzer::CalculateBurriedHolesAreaInVisibleRowsWithGravity(int landedPieceId) const {
    auto area = 0.0f;
    auto numColumns = mField.GetNumColumns();
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    
    for (auto column = 0; column < numColumns; ++column) {
        BurriedHoleScanStates states;
        
        for (auto row = lowestVisibleRow + mField.GetNumRowsInOneScreen() - 1;
             row >= lowestVisibleRow;
             --row) {
            auto& cell = mField.mGrid[row][column];
            
            switch (states.mState) {
                case HoleScanState::Idle:
                    if (cell.mIsInFilledRow) {
                        states = TransitionHoleState(HoleScanState::InsideFilledRow, states);
                    } else if (IsCellPartOfLandedPiece(cell, landedPieceId)) {
                        area += CalculateLowerHalfCellAreaContribution(cell);
                        states = TransitionHoleState(HoleScanState::InsideLandedPiece, states);
                    } else {
                        if (!cell.IsEmpty()) {
                            area += CalculateLowerHalfCellAreaContribution(cell);
                            states = TransitionHoleState(HoleScanState::AboveIsFilled, states);
                        }
                    }
                    break;
                case HoleScanState::InsideFilledRow:
                    if (cell.mIsInFilledRow) {
                        break;
                    } else if (IsCellPartOfLandedPiece(cell, landedPieceId)) {
                        area += CalculateAreaContributionUnderFilledRow(cell, states.mPreviousState);
                        states = TransitionHoleState(
                            HoleScanState::LandedPieceSitsOnOrHangsUnderFilledRow, states);
                    } else if (cell.IsEmpty()) {
                        switch (states.mPreviousState) {
                            case HoleScanState::InsideFilledRow:
                            case HoleScanState::InsideLandedPiece:
                            case HoleScanState::LandedPieceSitsOnOrHangsUnderFilledRow:
                                assert(false);
                            case HoleScanState::Idle:
                                states = TransitionHoleState(HoleScanState::Idle, states);
                                break;
                            case HoleScanState::AboveIsFilled:
                                area += 1.0f;
                                states = TransitionHoleState(HoleScanState::AboveIsFilled, states);
                                break;
                        }
                    } else {
                        area += CalculateAreaContributionUnderFilledRow(cell, states.mPreviousState);
                        states = TransitionHoleState(HoleScanState::AboveIsFilled, states);
                    }
                    break;
                case HoleScanState::InsideLandedPiece:
                    if (cell.mIsInFilledRow) {
                        states = TransitionHoleState(
                            HoleScanState::LandedPieceSitsOnOrHangsUnderFilledRow, states);
                    } else if (IsCellPartOfLandedPiece(cell, landedPieceId)) {
                        area += CalculateLowerHalfCellAreaContribution(cell);
                    } else {
                        area += CalculateAreaContribution(cell);
                        states = TransitionHoleState(HoleScanState::AboveIsFilled, states);
                    }
                    break;
                case HoleScanState::LandedPieceSitsOnOrHangsUnderFilledRow:
                    if (cell.mIsInFilledRow) {
                        break;
                    } else if (IsCellPartOfLandedPiece(cell, landedPieceId)) {
                        area += CalculateLowerHalfCellAreaContribution(cell);
                    } else {
                        if (!cell.IsEmpty()) {
                            area += CalculateLowerHalfCellAreaContribution(cell);
                            states = TransitionHoleState(HoleScanState::AboveIsFilled, states);
                        }
                    }
                    break;
                case HoleScanState::AboveIsFilled:
                    if (cell.mIsInFilledRow) {
                        states = TransitionHoleState(HoleScanState::InsideFilledRow, states);
                    } else if (IsCellPartOfLandedPiece(cell, landedPieceId)) {
                        area += CalculateAreaContribution(cell);
                        states = TransitionHoleState(HoleScanState::InsideLandedPiece, states);
                    } else {
                        area += CalculateAreaContribution(cell);
                    }
                    break;
            }
        }
    }
    
    return area;
}

float FieldAnalyzer::CalculateWellsAreaInVisibleRows() const {
    auto area = 0.0f;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    auto numColumns = mField.GetNumColumns();
    
    for (auto column = 0; column < numColumns; ++column) {
        auto leftColumnIndex = column - 1;
        auto rightColumnIndex = column + 1;
        
        for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; ++row) {
            auto& cell = mField.mGrid[row][column];
            if (!cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty =
                leftColumnIndex >= 0 ? mField.mGrid[row][leftColumnIndex].IsEmpty() : false;
            
            auto rightIsEmpty =
                rightColumnIndex < numColumns ? mField.mGrid[row][rightColumnIndex].IsEmpty() : false;
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
}

int FieldAnalyzer::CalculateNumTransitionsInVisibleRows() const {
    return CalculateNumTransitionsInColumns() + CalculateNumTransitionsInRows();
}

int FieldAnalyzer::CalculateNumTransitionsInColumns() const {
    auto numTransitions = 0;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    auto numColumns = mField.GetNumColumns();
    
    for (auto column = 0; column < numColumns; ++column) {
        auto previousCellIsEmpty = false;
        
        for (auto row = lowestVisibleRow; row < pastHighestVisibleRow - 1; ++row) {
            auto& cell = mField.mGrid[row][column];
            if (cell.mIsInFilledRow) {
                continue;
            }
            
            auto thisCellIsEmpty = cell.IsEmpty();
            if (thisCellIsEmpty != previousCellIsEmpty) {
                ++numTransitions;
            }
            
            previousCellIsEmpty = thisCellIsEmpty;
        }
    }
    
    return numTransitions;
}

int FieldAnalyzer::CalculateNumTransitionsInRows() const {
    auto numTransitions = 0;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    auto numColumns = mField.GetNumColumns();
    
    for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; ++row) {
        if (mField.mGrid[row][0].mIsInFilledRow) {
            continue;
        }
        
        for (auto column = 0; column < numColumns - 1; ++column) {
            auto thisCellIsEmpty = mField.mGrid[row][column].IsEmpty();
            auto nextCellIsEmpty = mField.mGrid[row][column + 1].IsEmpty();
            if (thisCellIsEmpty != nextCellIsEmpty) {
                ++numTransitions;
            }
        }
        
        auto leftCellIsEmpty = mField.mGrid[row][0].IsEmpty();
        if (leftCellIsEmpty) {
            ++numTransitions;
        }
        
        auto rightCellIsEmpty = mField.mGrid[row][numColumns - 1].IsEmpty();
        if (rightCellIsEmpty) {
            ++numTransitions;
        }
    }
    
    return numTransitions;
}

int FieldAnalyzer::CalculateNumCellsAccordingToBlueprintInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto result = 0;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    auto numColumns = mField.GetNumColumns();
    
    for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; ++row) {
        for (auto column = 0; column < numColumns; ++column) {
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

float FieldAnalyzer::CalculateBuildHolesAreaInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto area = 0.0f;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto numColumns = mField.GetNumColumns();
    
    for (auto column = 0; column < numColumns; ++column) {
        auto aboveIsFilled = false;
        
        for (auto row = lowestVisibleRow + mField.GetNumRowsInOneScreen() - 1;
             row >= lowestVisibleRow;
             --row) {
            auto& cell = mField.mGrid[row][column];
            if (cell.mIsInFilledRow) {
                continue;
            }
            
            auto thisCellShouldBeFilled =
                (*mField.mBlueprintGrid)[row][column].mFill != Fill::Empty;
            
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
            
            auto cellFill =
                cell.mSecondSubCell.IsEmpty() ? cell.mFirstSubCell.mFill : cell.mSecondSubCell.mFill;
            
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

float FieldAnalyzer::CalculateBuildWellsAreaInVisibleRows() const {
    assert(mField.mBlueprintGrid);
    
    auto area = 0.0f;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();
    auto numColumns = mField.GetNumColumns();
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    
    for (auto column = 0; column < numColumns; ++column) {
        auto leftColumnIndex = column - 1;
        auto rightColumnIndex = column + 1;
        
        for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; ++row) {
            auto& cell = mField.mGrid[row][column];
            
            auto thisCellShouldBeFilled =
                (*mField.mBlueprintGrid)[row][column].mFill != Fill::Empty;
            
            if (!thisCellShouldBeFilled || !cell.IsEmpty() || cell.mIsInFilledRow) {
                continue;
            }
            
            auto leftIsEmpty =
                leftColumnIndex >= 0 ? mField.mGrid[row][leftColumnIndex].IsEmpty() : false;
            
            auto rightIsEmpty =
                rightColumnIndex < numColumns ? mField.mGrid[row][rightColumnIndex].IsEmpty() : false;
            
            if (!leftIsEmpty && !rightIsEmpty) {
                area += 1.0f;
            }
        }
    }
    
    return area;
}
