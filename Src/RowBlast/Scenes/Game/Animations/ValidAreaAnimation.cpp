#include "ValidAreaAnimation.hpp"

// Game includes.
#include "Field.hpp"
#include "Piece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto validCell = 1;
    constexpr auto invalidCell = 0;
}

ValidAreaAnimation::ValidAreaAnimation(Field& field) :
    mField {field} {}

void ValidAreaAnimation::Init() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();
    
    mGrid.clear();
    mGrid.reserve(numRows);
    
    std::vector<int> row(numColumns);
    
    for (auto rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        mGrid.push_back(row);
    }
    
    ClearGrid();
    Stop();
}

void ValidAreaAnimation::Start(const Moves& allValidMoves,
                               const Piece& pieceType,
                               Rotation rotation) {
    mState = State::Active;
    ClearGrid();
    
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto fieldNumRows = mField.GetNumRows();
    
    for (auto& move: allValidMoves) {
        if (rotation != move.mRotation) {
            continue;
        }
        
        auto& pieceGrid = pieceType.GetGrid(move.mRotation);
        auto& movePosition = move.mPosition;
        
        for (auto pieceGridRow = 0; pieceGridRow < pieceNumRows; ++pieceGridRow) {
            for (auto pieceGridColumn = 0; pieceGridColumn < pieceNumColumns; ++pieceGridColumn) {
                auto& subCell = pieceGrid[pieceGridRow][pieceGridColumn].mFirstSubCell;
                if (subCell.mBlockKind == BlockKind::None) {
                    continue;
                }
                
                auto column = movePosition.x + pieceGridColumn;
                for (auto row = movePosition.y + pieceGridRow; row < fieldNumRows; ++row) {
                    auto cell = mGrid[row][column];
                    if (cell == validCell || !mField.GetCell(row, column).IsEmpty()) {
                        break;
                    }
                    
                    mGrid[row][column] = validCell;
                }
            }
        }
    }
}

void ValidAreaAnimation::Stop() {
    mState = State::Inactive;
}

bool ValidAreaAnimation::IsActive() const {
    return mState == State::Active;
}

bool ValidAreaAnimation::IsCellValid(int row, int column) const {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();

    assert(row < numRows && column < numColumns);
    return mGrid[row][column] == validCell;
}

void ValidAreaAnimation::ClearGrid() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();
    
    for (auto column = 0; column < numColumns; ++column) {
        for (auto row = 0; row < numRows; ++row) {
            mGrid[row][column] = invalidCell;
        }
    }
}
