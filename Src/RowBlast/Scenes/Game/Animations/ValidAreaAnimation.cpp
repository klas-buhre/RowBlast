#include "ValidAreaAnimation.hpp"

// Game includes.
#include "Piece.hpp"
#include "ScrollController.hpp"

using namespace RowBlast;

namespace {
    bool ShouldProcessMove(const Move& move, const Piece& pieceType, Rotation rotation) {
        auto& duplicateMoveCheck = pieceType.GetDuplicateMoveCheck(rotation);
        if (!duplicateMoveCheck.HasValue()) {
            return move.mRotation == rotation;
        }
        
        auto& duplicateMoveCheckValue = duplicateMoveCheck.GetValue();
        return move.mRotation == rotation || move.mRotation == duplicateMoveCheckValue.mRotation;
    }
}

ValidAreaAnimation::ValidAreaAnimation(Field& field, const ScrollController& scrollController) :
    mField {field},
    mScrollController {scrollController} {}

void ValidAreaAnimation::Init() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();
    
    mGrid.clear();
    mGrid.reserve(numRows);
    std::vector<CellStatus> row {static_cast<size_t>(numColumns), CellStatus::Undefined};
    for (auto rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        mGrid.push_back(row);
    }

    mSearchData.clear();
    mSearchData.reserve(numRows);
    std::vector<ValidMoveBelow> searchDataRow {static_cast<size_t>(numColumns), ValidMoveBelow::Undefined};
    for (auto rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        mSearchData.push_back(searchDataRow);
    }

    ClearGrids();
    Stop();
}

void ValidAreaAnimation::Start(const Moves& allValidMoves,
                               const Piece& pieceType,
                               Rotation rotation) {
    mState = State::Active;
    ClearGrids();
    
    for (auto& move: allValidMoves) {
        if (ShouldProcessMove(move, pieceType, rotation)) {
            FillValidAreaAboveMove(move, pieceType);
        }
    }
    
    InvalidMoves invalidMoves;
    FindInvalidMoves(invalidMoves, pieceType, rotation);
}

void ValidAreaAnimation::FillValidAreaAboveMove(const Move& move, const Piece& pieceType) {
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto fieldNumRows = mField.GetNumRows();
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
                if (cell == CellStatus::Valid || mField.GetCell(row, column).IsFull()) {
                    break;
                }
                
                mGrid[row][column] = CellStatus::Valid;
            }
        }
    }
}

void ValidAreaAnimation::FindInvalidMoves(InvalidMoves& invalidMoves,
                                          const Piece& pieceType,
                                          Rotation rotation) {
    auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();

    auto& pieceDimensions = pieceType.GetDimensions(rotation);
    auto startColumn = -pieceDimensions.mXmin;
    auto endColumn = mField.GetNumColumns() - pieceDimensions.mXmax;
    auto endRow = lowestVisibleRow - pieceDimensions.mYmin;
    auto startRow = pastHighestVisibleRow - pieceDimensions.mYmax;
    
    PieceBlocks pieceBlocks {
        pieceType.GetGrid(rotation),
        pieceType.GetGridNumRows(),
        pieceType.GetGridNumColumns()
    };

    for (auto row = startRow; row >= endRow; --row) {
        for (auto column = startColumn; column < endColumn; ++column) {
            Pht::IVec2 movePosition {row, column};
            Field::CollisionResult collisionResult;
            mField.CheckCollision(collisionResult,
                                  pieceBlocks,
                                  movePosition,
                                  Pht::IVec2{0, 0},
                                  false);
            if (collisionResult.mIsCollision == IsCollision::Yes) {

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

bool ValidAreaAnimation::IsCellInvalid(int row, int column) const {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();

    assert(row < numRows && column < numColumns);
    return mGrid[row][column] == CellStatus::Invalid;
}

void ValidAreaAnimation::ClearGrids() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();
    
    for (auto column = 0; column < numColumns; ++column) {
        for (auto row = 0; row < numRows; ++row) {
            mGrid[row][column] = CellStatus::Undefined;
            mSearchData[row][column] = ValidMoveBelow::Undefined;
        }
    }
}
