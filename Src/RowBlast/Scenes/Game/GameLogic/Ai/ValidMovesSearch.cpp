#include "ValidMovesSearch.hpp"

#include <assert.h>

// Game includes.
#include "CollisionDetection.hpp"

using namespace RowBlast;

namespace {
    const auto maxRotateAdjustment {2};
    
    const Movement* AddMovement(ValidMoves& validMoves,
                                const MovingPiece& piece,
                                const Movement* previousMovement) {
        auto& position = piece.mPosition;
        Pht::Vec2 floatPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
        Movement movement {floatPosition, piece.mRotation, previousMovement};
        validMoves.mMovements.PushBack(movement);
        return &validMoves.mMovements.Back();
    }
    
    int CalcNumMovements(const Movement& finalMovement) {
        auto numMovements = 0;
        for (auto* movement = &finalMovement; movement; movement = movement->GetPrevious()) {
            ++numMovements;
        }
        
        return numMovements;
    }
    
    PieceBlocks CreatePieceBlocks(const MovingPiece& piece) {
        auto& pieceType = piece.mPieceType;
        
        return PieceBlocks {
            pieceType.GetGrid(piece.mRotation),
            pieceType.GetGridNumRows(),
            pieceType.GetGridNumColumns()
        };
    }
    
    bool CellFillsRightSide(const Cell& cell) {
        return cell.mFirstSubCell.FillsRightCellSide() || cell.mSecondSubCell.FillsRightCellSide();
    }
    
    bool CellFillsLeftSide(const Cell& cell) {
        return cell.mFirstSubCell.FillsLeftCellSide() || cell.mSecondSubCell.FillsLeftCellSide();
    }
}

ValidMovesSearch::ValidMovesSearch(Field& field) :
    mField {field} {}

void ValidMovesSearch::Init() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();
    
    mSearchGrid.clear();
    mSearchGrid.reserve(numRows);
        
    std::vector<CellSearchData> row(numColumns);
        
    for (auto rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        mSearchGrid.push_back(row);
    }
    
    mValidArea.clear();
    mValidArea.reserve(numRows);
        
    std::vector<int> validAreaRow(numColumns);
        
    for (auto rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        mValidArea.push_back(validAreaRow);
    }
}

void ValidMovesSearch::FindValidMoves(ValidMoves& validMoves,
                                      MovingPiece piece,
                                      const Level::TutorialMove* predeterminedMove,
                                      const std::vector<Level::TutorialMove>* suggestedMoves) {
    mPredeterminedMove = predeterminedMove;
    mSuggestedMoves = suggestedMoves;

    InitSearchGrid();
    ResetValidArea();
    FindMostValidMovesWithHumanLikeSearch(validMoves, piece);
    ResetVisitedLocations();
    FindMostRemainingValidMoves(validMoves, piece);
    FindRemainingValidMovesConnectedToValidArea(validMoves, piece);
}

void ValidMovesSearch::InitSearchGrid() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();

    enum class OverhangScanState {
        FreeSpace,
        OccupiedSpace
    };
    
    for (auto column = 0; column < numColumns; ++column) {
        auto overhangScanState = OverhangScanState::FreeSpace;
        
        for (auto row = numRows - 1; row >= 0; --row) {
            auto& cellSearchData = mSearchGrid[row][column];
            auto& searchDataForRotations = cellSearchData.mData;
            
            for (auto& searchData: searchDataForRotations) {
                searchData = SearchDataForOneRotation {};
            }

            cellSearchData.mFirstMovementAtLocation = nullptr;
            cellSearchData.mUnderOverhangTip = false;
            
            auto& fieldCell = mField.GetCell(row, column);
            
            switch (overhangScanState) {
                case OverhangScanState::FreeSpace:
                    if (!fieldCell.IsEmpty()) {
                        auto& firstSubCell = fieldCell.mFirstSubCell;
                        auto& secondSubCell = fieldCell.mSecondSubCell;
                        auto leftColumn = column - 1;
                        auto rightColumn = column + 1;
                        
                        if (firstSubCell.mFill == Fill::UpperRightHalf && secondSubCell.IsEmpty() &&
                            leftColumn >= 0 && mField.GetCell(row, leftColumn).IsEmpty()) {
                            cellSearchData.mUnderOverhangTip = true;
                        }

                        if (secondSubCell.mFill == Fill::UpperRightHalf && firstSubCell.IsEmpty() &&
                            leftColumn >= 0 && mField.GetCell(row, leftColumn).IsEmpty()) {
                            cellSearchData.mUnderOverhangTip = true;
                        }

                        if (firstSubCell.mFill == Fill::UpperLeftHalf && secondSubCell.IsEmpty() &&
                            rightColumn < numColumns &&
                            mField.GetCell(row, rightColumn).IsEmpty()) {
                            cellSearchData.mUnderOverhangTip = true;
                        }
                        
                        if (secondSubCell.mFill == Fill::UpperLeftHalf && firstSubCell.IsEmpty() &&
                            rightColumn < numColumns &&
                            mField.GetCell(row, rightColumn).IsEmpty()) {
                            cellSearchData.mUnderOverhangTip = true;
                        }
                        
                        overhangScanState = OverhangScanState::OccupiedSpace;
                    }
                    break;
                case OverhangScanState::OccupiedSpace:
                    if (!fieldCell.IsFull()) {
                        auto rowAbove = row + 1;
                        
                        if (rowAbove < numRows) {
                            auto leftColumn = column - 1;
                            
                            if (leftColumn >= 0 && mField.GetCell(rowAbove, leftColumn).IsEmpty() &&
                                !CellFillsLeftSide(fieldCell) &&
                                !CellFillsRightSide(mField.GetCell(row, leftColumn))) {
                                cellSearchData.mUnderOverhangTip = true;
                            }
                            
                            auto rightColumn = column + 1;
                            
                            if (rightColumn < numColumns &&
                                mField.GetCell(rowAbove, rightColumn).IsEmpty() &&
                                !CellFillsRightSide(fieldCell) &&
                                !CellFillsLeftSide(mField.GetCell(row, rightColumn))) {
                                cellSearchData.mUnderOverhangTip = true;
                            }
                            
                            if (fieldCell.IsEmpty()) {
                                overhangScanState = OverhangScanState::FreeSpace;
                            }
                        }
                    }
                    break;
            }
        }
    }
}

void ValidMovesSearch::ResetValidArea() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();

    for (auto row = 0; row < numRows; ++row) {
        for (auto column = 0; column < numColumns; ++column) {
            mValidArea[row][column] = invalidCell;
        }
    }
}

void ValidMovesSearch::ResetVisitedLocations() {
    auto numRows = mField.GetNumRows();
    auto numColumns = mField.GetNumColumns();

    for (auto row = 0; row < numRows; ++row) {
        for (auto column = 0; column < numColumns; ++column) {
            auto& searchDataForRotations = mSearchGrid[row][column].mData;
            for (auto& searchData: searchDataForRotations) {
                searchData.mIsVisited = false;
            }
        }
    }
}

void ValidMovesSearch::AdjustPosition(MovingPiece& piece) {
    if (!piece.mPieceType.PositionCanBeAdjusteInMovesSearch()) {
        return;
    }

    const auto& position = piece.mPosition;
    auto pieceBlocks = CreatePieceBlocks(piece);
    
    mField.CheckCollision(mCollisionResult,
                          pieceBlocks,
                          position,
                          Pht::IVec2{0, 0},
                          false,
                          &mValidArea);
    if (mCollisionResult.mIsCollision == IsCollision::Yes) {
        auto collisionDirection =
            CollisionDetection::CalculateCollisionDirection(mCollisionResult.mCollisionPoints,
                                                            pieceBlocks,
                                                            piece.mRotation,
                                                            position,
                                                            mField);
        
        switch (collisionDirection) {
            case Direction::Right: {
                auto freeXPos = mField.DetectFreeSpaceLeft(pieceBlocks, position);
                if (position.x - freeXPos <= maxRotateAdjustment) {
                    piece.mPosition.x = freeXPos;
                }
                break;
            }
            case Direction::Left: {
                auto freeXPos = mField.DetectFreeSpaceRight(pieceBlocks, position);
                if (freeXPos - position.x <= maxRotateAdjustment) {
                    piece.mPosition.x = freeXPos;
                }
                break;
            }
            case Direction::Up: {
                auto freeYPos = mField.DetectFreeSpaceDown(pieceBlocks, position);
                if (position.y - freeYPos <= maxRotateAdjustment) {
                    piece.mPosition.y = freeYPos;
                }
                break;
            }
            case Direction::Down: {
                auto freeYPos = mField.DetectFreeSpaceUp(pieceBlocks, position);
                if (freeYPos - position.y <= maxRotateAdjustment) {
                    piece.mPosition.y = freeYPos;
                }
                break;
            }
            default:
                break;
        }
    }
}

void ValidMovesSearch::FindMostValidMovesWithHumanLikeSearch(ValidMoves& validMoves,
                                                             MovingPiece piece) {
    FindValidMoves(validMoves,
                   piece,
                   SearchDirection::Left,
                   nullptr,
                   AllowRecursion::Yes,
                   PositionAdjustment::Yes);
    FindValidMoves(validMoves,
                   piece,
                   SearchDirection::Right,
                   nullptr,
                   AllowRecursion::Yes,
                   PositionAdjustment::Yes);
    
    auto numRotations = piece.mPieceType.GetNumRotations();
    
    for (auto rotation = 0; rotation < numRotations; ++rotation) {
        // TODO: Try anti-clockwise also?
        piece.RotateClockwise();
        AdjustPosition(piece);
        
        if (IsCollision(piece)) {
            continue;
        }
        
        auto* rotationMovement = AddMovement(validMoves, piece, nullptr);
        
        SearchDown(validMoves,
                   piece,
                   rotationMovement,
                   AllowRecursion::Yes,
                   PositionAdjustment::No);
    }
}

void ValidMovesSearch::FindValidMoves(ValidMoves& validMoves,
                                      MovingPiece piece,
                                      SearchDirection searchDirection,
                                      const Movement* previousMovement,
                                      AllowRecursion allowRecursion,
                                      PositionAdjustment positionAdjustment) {
    switch (searchDirection) {
        case SearchDirection::Left:
            --piece.mPosition.x;
            break;
        case SearchDirection::Right:
            ++piece.mPosition.x;
            break;
    }
    
    auto numRotations = piece.mPieceType.GetNumRotations();
    auto rotationAtEntry = piece.mRotation;
    
    for (auto rotation = 0; rotation < numRotations; ++rotation) {
        if (!FindValidMovesForRotation(validMoves,
                                       piece,
                                       searchDirection,
                                       previousMovement,
                                       allowRecursion,
                                       positionAdjustment)) {
            break;
        }
        
        piece.RotateClockwise();
    }
    
    piece.mRotation = rotationAtEntry;
    
    for (auto rotation = 0; rotation < numRotations; ++rotation) {
        if (!FindValidMovesForRotation(validMoves,
                                       piece,
                                       searchDirection,
                                       previousMovement,
                                       allowRecursion,
                                       positionAdjustment)) {
            break;
        }

        piece.RotateAntiClockwise();
    }
}

bool ValidMovesSearch::FindValidMovesForRotation(ValidMoves& validMoves,
                                                 MovingPiece piece,
                                                 SearchDirection searchDirection,
                                                 const Movement* previousMovement,
                                                 AllowRecursion allowRecursion,
                                                 PositionAdjustment positionAdjustment) {
    if (positionAdjustment == PositionAdjustment::Yes) {
        AdjustPosition(piece);
    } else if (IsCollision(piece)) {
        return false;
    }

    if (IsLocationVisited(piece)) {
        return true;
    }

    auto* rotationMovement = AddMovement(validMoves, piece, previousMovement);

    switch (searchDirection) {
        case SearchDirection::Left:
            SearchLeft(validMoves, piece, rotationMovement, allowRecursion, positionAdjustment);
            break;
        case SearchDirection::Right:
            SearchRight(validMoves, piece, rotationMovement, allowRecursion, positionAdjustment);
            break;
    }

    MarkLocationAsVisited(piece);
    return true;
}

void ValidMovesSearch::SearchLeft(ValidMoves& validMoves,
                                  MovingPiece piece,
                                  const Movement* previousMovement,
                                  AllowRecursion allowRecursion,
                                  PositionAdjustment positionAdjustment) {
    if (IsCollision(piece)) {
        return;
    }
    
    auto xBegin = piece.mPosition.x;
    auto xEnd = HandleCollisionLeft(piece);
    
    for (auto x = xBegin; x >= xEnd; --x) {
        piece.mPosition.x = x;
        auto* newPositionMovement = AddMovement(validMoves, piece, previousMovement);
        
        SearchDown(validMoves, piece, newPositionMovement, allowRecursion, positionAdjustment);
    }
}

void ValidMovesSearch::SearchRight(ValidMoves& validMoves,
                                   MovingPiece piece,
                                   const Movement* previousMovement,
                                   AllowRecursion allowRecursion,
                                   PositionAdjustment positionAdjustment) {
    if (IsCollision(piece)) {
        return;
    }
    
    auto xBegin = piece.mPosition.x;
    auto xEnd = HandleCollisionRight(piece);
    
    for (auto x = xBegin; x <= xEnd; ++x) {
        piece.mPosition.x = x;
        auto* newPositionMovement = AddMovement(validMoves, piece, previousMovement);
        
        SearchDown(validMoves, piece, newPositionMovement, allowRecursion, positionAdjustment);
    }
}

void ValidMovesSearch::SearchDown(ValidMoves& validMoves,
                                  MovingPiece piece,
                                  const Movement* previousMovement,
                                  AllowRecursion allowRecursion,
                                  PositionAdjustment positionAdjustment) {
    
    auto yBegin = piece.mPosition.y;
    auto yEnd = HandleCollisionDown(piece);
    
    auto pieceTypeRightOverhangCheckPos =
        piece.mPieceType.GetRightOverhangCheckPosition(piece.mRotation);
    
    auto pieceTypeLeftOverhangCheckPos =
        piece.mPieceType.GetLeftOverhangCheckPosition(piece.mRotation);
    
    auto pieceTypeRightExtremityCheckPos =
        piece.mPieceType.GetRightExtremityCheckPosition(piece.mRotation);

    auto pieceTypeLeftExtremityCheckPos =
        piece.mPieceType.GetLeftExtremityCheckPosition(piece.mRotation);
    
    for (auto y = yBegin; y >= yEnd; --y) {
        piece.mPosition.y = y;
        
        if (allowRecursion == AllowRecursion::Yes) {
            if (pieceTypeRightExtremityCheckPos.HasValue()) {
                Pht::IVec2 rightExtremityCheckPos {
                    piece.mPosition + pieceTypeRightExtremityCheckPos.GetValue()
                };
                
                if (rightExtremityCheckPos.x < mField.GetNumColumns() &&
                    mSearchGrid[rightExtremityCheckPos.y][rightExtremityCheckPos.x].mUnderOverhangTip) {
                    FindValidMoves(validMoves,
                                   piece,
                                   SearchDirection::Right,
                                   previousMovement,
                                   AllowRecursion::No,
                                   PositionAdjustment::No);
                }
            }
            
            if (pieceTypeLeftExtremityCheckPos.HasValue()) {
                Pht::IVec2 leftExtremityCheckPos {
                    piece.mPosition + pieceTypeLeftExtremityCheckPos.GetValue()
                };
                
                if (leftExtremityCheckPos.x >= 0 &&
                    mSearchGrid[leftExtremityCheckPos.y][leftExtremityCheckPos.x].mUnderOverhangTip) {
                    FindValidMoves(validMoves,
                                   piece,
                                   SearchDirection::Left,
                                   previousMovement,
                                   AllowRecursion::No,
                                   PositionAdjustment::No);
                }
            }
            
            Pht::IVec2 rightOverhangCheckPos {piece.mPosition + pieceTypeRightOverhangCheckPos};
            
            if (rightOverhangCheckPos.x < mField.GetNumColumns() &&
                mSearchGrid[rightOverhangCheckPos.y][rightOverhangCheckPos.x].mUnderOverhangTip &&
                y != yBegin) {
                FindValidMoves(validMoves,
                               piece,
                               SearchDirection::Right,
                               previousMovement,
                               AllowRecursion::Yes,
                               PositionAdjustment::No);
            }
            
            Pht::IVec2 leftOverhangCheckPos {piece.mPosition + pieceTypeLeftOverhangCheckPos};
            
            if (leftOverhangCheckPos.x >= 0 &&
                mSearchGrid[leftOverhangCheckPos.y][leftOverhangCheckPos.x].mUnderOverhangTip &&
                y != yBegin) {
                FindValidMoves(validMoves,
                               piece,
                               SearchDirection::Left,
                               previousMovement,
                               AllowRecursion::Yes,
                               PositionAdjustment::No);
            }
        }
    }
    
    piece.mPosition.y = yEnd;
    
    SaveMove(validMoves, piece, previousMovement);
}

void ValidMovesSearch::SaveMove(ValidMoves& validMoves,
                                const MovingPiece& piece,
                                const Movement* previousMovement) {
    if (IsDuplicateMoveFoundAtDifferentLocation(piece) ||
        CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                        piece.mPosition,
                                                        piece.mRotation,
                                                        piece.mPieceType)) {
        return;
    }
    
    if (IsMoveDiscardedByTutorial(piece)) {
        return;
    }
    
    if (auto* foundMove = GetFoundMove(piece)) {
        auto foundMoveNumMovements = CalcNumMovements(*foundMove->mLastMovement);
        auto thisMoveNumMovements = CalcNumMovements(*previousMovement) + 1;

        if (thisMoveNumMovements < foundMoveNumMovements) {
            auto* lastMovement = AddMovement(validMoves, piece, previousMovement);
            *foundMove = Move {piece.mPosition, piece.mRotation, lastMovement};
        }
    } else {
        auto* lastMovement = AddMovement(validMoves, piece, previousMovement);
        Move move {piece.mPosition, piece.mRotation, lastMovement};
    
        validMoves.mMoves.PushBack(move);
        SetFoundMove(piece, validMoves.mMoves.Back());
    }
}

bool ValidMovesSearch::IsMoveDiscardedByTutorial(const MovingPiece& piece) const {
    if (mPredeterminedMove && mSuggestedMoves) {
        if (piece.IsAtTutorialMove(*mPredeterminedMove)) {
            return false;
        }
        
        for (auto& suggestedMove: *mSuggestedMoves) {
            if (piece.IsAtTutorialMove(suggestedMove)) {
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

void ValidMovesSearch::FindMostRemainingValidMoves(ValidMoves& validMoves, MovingPiece piece) {
    Search(validMoves, piece, nullptr, SearchMovement::Start);
}

void ValidMovesSearch::Search(ValidMoves& validMoves,
                              MovingPiece piece,
                              const Movement* previousMovement,
                              SearchMovement searchMovement) {
    if (!MovePieceAndCheckEdges(piece, searchMovement)) {
        return;
    }

    if (IsLocationVisited(piece)) {
        return;
    }

    switch (HandleCollision(piece, searchMovement)) {
        case SearchCollisionResult::Collision:
            return;
        case SearchCollisionResult::FoundMove:
            SaveMoveIfNotFoundBefore(validMoves, piece, previousMovement);
            break;
        case SearchCollisionResult::NoCollision:
            break;
    }
    
    MarkLocationAsVisited(piece);

    auto* movement = AddMovementAndRemoveDetour(validMoves, piece, previousMovement);

    Search(validMoves, piece, movement, SearchMovement::Down);
    Search(validMoves, piece, movement, SearchMovement::Right);
    Search(validMoves, piece, movement, SearchMovement::Left);
    Search(validMoves, piece, movement, SearchMovement::RotateClockwise);
    Search(validMoves, piece, movement, SearchMovement::RotateAntiClockwise);
}

bool ValidMovesSearch::MovePieceAndCheckEdges(MovingPiece& piece, SearchMovement searchMovement) {
    switch (searchMovement) {
        case SearchMovement::Down: {
            piece.mPosition.y--;
            auto& pieceDimensions = piece.mPieceType.GetDimensions(piece.mRotation);
            if (piece.mPosition.y + pieceDimensions.mYmin < mField.GetLowestVisibleRow()) {
                return false;
            }
            break;
        }
        case SearchMovement::Right: {
            piece.mPosition.x++;
            auto& pieceDimensions = piece.mPieceType.GetDimensions(piece.mRotation);
            if (piece.mPosition.x + pieceDimensions.mXmax >= mField.GetNumColumns()) {
                return false;
            }
            break;
        }
        case SearchMovement::Left: {
            piece.mPosition.x--;
            auto& pieceDimensions = piece.mPieceType.GetDimensions(piece.mRotation);
            if (piece.mPosition.x + pieceDimensions.mXmin < 0) {
                return false;
            }
            break;
        }
        case SearchMovement::RotateClockwise:
            piece.RotateClockwise();
            break;
        case SearchMovement::RotateAntiClockwise:
            piece.RotateAntiClockwise();
            break;
        case SearchMovement::Start:
            break;
    }
    
    return true;
}

ValidMovesSearch::SearchCollisionResult
ValidMovesSearch::HandleCollision(const MovingPiece& piece, SearchMovement searchMovement) {
    switch (searchMovement) {
        case SearchMovement::Down: {
            auto piecePreviousState = piece;
            piecePreviousState.mPosition.y++;
            auto collisionRow = HandleCollisionDown(piecePreviousState);
            if (collisionRow == piece.mPosition.y) {
                return SearchCollisionResult::FoundMove;
            } else if (collisionRow == piecePreviousState.mPosition.y) {
                return SearchCollisionResult::Collision;
            }
            break;
        }
        case SearchMovement::Right: {
            auto piecePreviousState = piece;
            piecePreviousState.mPosition.x--;
            if (HandleCollisionRight(piecePreviousState) == piecePreviousState.mPosition.x) {
                return SearchCollisionResult::Collision;
            }
            if (HandleCollisionDown(piece) == piece.mPosition.y) {
                return SearchCollisionResult::FoundMove;
            }
            break;
        }
        case SearchMovement::Left: {
            auto piecePreviousState = piece;
            piecePreviousState.mPosition.x++;
            if (HandleCollisionLeft(piecePreviousState) == piecePreviousState.mPosition.x) {
                return SearchCollisionResult::Collision;
            }
            if (HandleCollisionDown(piece) == piece.mPosition.y) {
                return SearchCollisionResult::FoundMove;
            }
            break;
        }
        case SearchMovement::RotateClockwise:
        case SearchMovement::RotateAntiClockwise:
        case SearchMovement::Start:
            if (IsCollision(piece)) {
                return SearchCollisionResult::Collision;
            }
            if (HandleCollisionDown(piece) == piece.mPosition.y) {
                return SearchCollisionResult::FoundMove;
            }
            break;
    }
    
    return SearchCollisionResult::NoCollision;
}

void ValidMovesSearch::FindRemainingValidMovesConnectedToValidArea(ValidMoves& validMoves,
                                                                   MovingPiece piece) {
    auto numRotations = piece.mPieceType.GetNumRotations();
    auto rowBegin = piece.mPosition.y;
    auto lowestVisibleRow = mField.GetLowestVisibleRow();

    for (auto rotation = 0; rotation < numRotations; ++rotation) {
        auto& pieceDimensions = piece.mPieceType.GetDimensions(piece.mRotation);
        auto columnBegin = -pieceDimensions.mXmin;
        auto columnEnd = mField.GetNumColumns() - pieceDimensions.mXmax;
        
        for (auto column = columnBegin; column < columnEnd; ++column) {
            piece.mPosition.x = column;
            
            for (auto row = rowBegin; row >= lowestVisibleRow;) {
                piece.mPosition.y = row;
                
                if (!IsCollision(piece, false)) {
                    row = HandleCollisionDown(piece, false);
                    piece.mPosition.y = row;
                    
                    if (IsConnectedToValidArea(piece) && IsMovePassingInitialCheck(piece)) {
                        auto* connectedMove = FindConnectedMove(validMoves, piece);
                        if (connectedMove) {
                            SaveMoveNoChecks(validMoves, piece, connectedMove->mLastMovement);
                        }
                    }
                }
                
                --row;
            }
        }
        
        piece.RotateClockwise();
    }
}

bool ValidMovesSearch::IsConnectedToValidArea(const MovingPiece& piece) const {
    auto& pieceType = piece.mPieceType;
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto& pieceGrid = pieceType.GetGrid(piece.mRotation);
    auto& position = piece.mPosition;

    for (auto pieceRow = 0; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto& pieceSubCell = pieceGrid[pieceRow][pieceColumn].mFirstSubCell;
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto fieldRow = position.y + pieceRow;
            auto fieldColumn = position.x + pieceColumn;
            if (fieldRow < mField.GetLowestVisibleRow() || fieldRow >= mField.GetNumRows() ||
                fieldColumn < 0 || fieldColumn >= mField.GetNumColumns()) {

                continue;
            }
            
            if (mValidArea[fieldRow][fieldColumn] == validCell) {
                return true;
            }
            
            if (fieldRow > mField.GetLowestVisibleRow() &&
                mValidArea[fieldRow - 1][fieldColumn] == validCell) {
                
                return true;
            }
            
            if (fieldColumn > 0 && mValidArea[fieldRow][fieldColumn - 1] == validCell) {
                return true;
            }
            
            if (fieldColumn < mField.GetNumColumns() - 2 &&
                mValidArea[fieldRow][fieldColumn + 1] == validCell) {
                
                return true;
            }
        }
    }

    return false;
}

const Move* ValidMovesSearch::FindConnectedMove(const ValidMoves& validMoves,
                                                const MovingPiece& piece) const {
    auto& moves = validMoves.mMoves;
    auto numMoves = moves.Size();

    for (auto i = 0; i < numMoves; ++i) {
        auto& move = moves.At(i);
        if (AreGridBoxesConnected(piece, move) && ArePiecesConnected(piece, move)) {
            return &move;
        }
    }
    
    return nullptr;
}

bool ValidMovesSearch::AreGridBoxesConnected(const MovingPiece& piece, const Move& move) const {
    auto piecePosition = piece.mPosition;
    auto movePosition = move.mPosition;
    auto& pieceType = piece.mPieceType;
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    
    return IsConnected1D(piecePosition.x, pieceNumColumns, movePosition.x, pieceNumColumns) &&
           IsConnected1D(piecePosition.y, pieceNumRows, movePosition.y, pieceNumRows);
}

bool ValidMovesSearch::IsConnected1D(int coord1, int size1, int coord2, int size2) const {
    return coord1 + size1 + 1 >= coord2 && coord2 + size2 >= coord1 - 1;
}

bool ValidMovesSearch::ArePiecesConnected(const MovingPiece& piece, const Move& move) const {
    auto piecePosition = piece.mPosition;
    auto movePosition = move.mPosition;
    auto& pieceType = piece.mPieceType;
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto& pieceGrid = pieceType.GetGrid(piece.mRotation);
    auto& moveGrid = pieceType.GetGrid(move.mRotation);
    
    for (auto pieceRow = 0; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto& pieceSubCell = pieceGrid[pieceRow][pieceColumn].mFirstSubCell;
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto rowDiff = piecePosition.y - movePosition.y;
            auto columnDiff = piecePosition.x - movePosition.x;
            auto moveRow = pieceRow + rowDiff;
            auto moveColumn = pieceColumn + columnDiff;

            if (moveColumn < pieceNumColumns && moveColumn >= 1 && moveRow >= 0 &&
                moveRow < pieceNumRows &&
                !moveGrid[moveRow][moveColumn - 1].mFirstSubCell.IsEmpty()) {
                
                return true;
            }

            if (moveColumn < pieceNumColumns - 2 && moveColumn >= -1 && moveRow >= 0 &&
                moveRow < pieceNumRows &&
                !moveGrid[moveRow][moveColumn + 1].mFirstSubCell.IsEmpty()) {
                
                return true;
            }
            
            if (moveRow < 0 || moveRow >= pieceNumRows || moveColumn < 0 ||
                moveColumn >= pieceNumColumns) {

                continue;
            }

            auto& moveSubCell = moveGrid[moveRow][moveColumn].mFirstSubCell;
            if (!moveSubCell.IsEmpty()) {
                return true;
            }
        }
    }
    
    return false;
}

void ValidMovesSearch::SaveMoveIfNotFoundBefore(ValidMoves& validMoves,
                                                const MovingPiece& piece,
                                                const Movement* previousMovement) {
    if (GetFoundMove(piece) || IsDuplicateMoveFoundAtDifferentLocation(piece) ||
        CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                        piece.mPosition,
                                                        piece.mRotation,
                                                        piece.mPieceType)) {
        return;
    }
    
    if (IsMoveDiscardedByTutorial(piece)) {
        return;
    }

    auto* lastMovement = AddMovement(validMoves, piece, previousMovement);
    Move move {piece.mPosition, piece.mRotation, lastMovement};

    validMoves.mMoves.PushBack(move);
    SetFoundMove(piece, validMoves.mMoves.Back());
}

void ValidMovesSearch::SaveMoveNoChecks(ValidMoves& validMoves,
                                        const MovingPiece& piece,
                                        const Movement* previousMovement) {
    auto* lastMovement = AddMovement(validMoves, piece, previousMovement);
    Move move {piece.mPosition, piece.mRotation, lastMovement};

    validMoves.mMoves.PushBack(move);
    SetFoundMove(piece, validMoves.mMoves.Back());
}

bool ValidMovesSearch::IsDuplicateMoveFoundAtDifferentLocation(const MovingPiece& piece) const {
    auto& duplicateMoveCheck = piece.mPieceType.GetDuplicateMoveCheck(piece.mRotation);
    if (duplicateMoveCheck.HasValue()) {
        auto& duplicateMoveCheckValue = duplicateMoveCheck.GetValue();
        auto checkPosition = piece.mPosition + duplicateMoveCheckValue.mRelativePosition;
        
        auto& searchData =
            GetSearchDataForOneRotation(checkPosition, duplicateMoveCheckValue.mRotation);
        
        if (searchData.mFoundMove) {
            return true;
        }
    }
    
    return false;
}

bool ValidMovesSearch::IsMovePassingInitialCheck(const MovingPiece& piece) const {
    if (GetFoundMove(piece) || IsDuplicateMoveFoundAtDifferentLocation(piece) ||
        CollisionDetection::IsIllegalTiltedBondPosition(mField,
                                                        piece.mPosition,
                                                        piece.mRotation,
                                                        piece.mPieceType)) {
        return false;
    }
    
    if (IsMoveDiscardedByTutorial(piece)) {
        return false;
    }

    return true;
}

const Movement* ValidMovesSearch::AddMovementAndRemoveDetour(ValidMoves& validMoves,
                                                             const MovingPiece& piece,
                                                             const Movement* previousMovement) {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    auto& cellSearchData = mSearchGrid[gridPosition.y][gridPosition.x];
    if (cellSearchData.mFirstMovementAtLocation) {
        return AddMovement(validMoves, piece, cellSearchData.mFirstMovementAtLocation);
    }
    
    auto* movement = AddMovement(validMoves, piece, previousMovement);
    cellSearchData.mFirstMovementAtLocation = movement;
    return movement;
}

bool ValidMovesSearch::IsLocationVisited(const MovingPiece& piece) const {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mIsVisited;
}

void ValidMovesSearch::MarkLocationAsVisited(const MovingPiece& piece) {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mIsVisited = true;
}

Move* ValidMovesSearch::GetFoundMove(const MovingPiece& piece) const {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mFoundMove;
}

void ValidMovesSearch::SetFoundMove(const MovingPiece& piece, Move& move) {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mFoundMove = &move;
}

ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const MovingPiece& piece) {
    auto gridPosition = CalculateSearchGridPosition(piece.mPosition);
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)];
}

ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const Pht::IVec2& position, Rotation rotation) {
    auto gridPosition = CalculateSearchGridPosition(position);
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(rotation)];
}

const ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const Pht::IVec2& position, Rotation rotation) const {
    auto gridPosition = CalculateSearchGridPosition(position);
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(rotation)];
}

Pht::IVec2 ValidMovesSearch::CalculateSearchGridPosition(const Pht::IVec2& position) const {
    return Pht::IVec2 {
        position.x >= 0 ? position.x : position.x + mField.GetNumColumns(),
        position.y >= 0 ? position.y : position.y + mField.GetNumRows()
    };
}

bool ValidMovesSearch::IsCollision(const MovingPiece& piece, bool fillValidArea) {
    const auto& position = piece.mPosition;
    auto pieceBlocks = CreatePieceBlocks(piece);
    
    mField.CheckCollision(mCollisionResult,
                          pieceBlocks,
                          position,
                          Pht::IVec2{0, 0},
                          false,
                          fillValidArea ? &mValidArea : nullptr);
    
    return mCollisionResult.mIsCollision == IsCollision::Yes;
}

int ValidMovesSearch::HandleCollisionLeft(const MovingPiece& piece) {
    auto& cellSearchData = GetSearchDataForOneRotation(piece);
    if (cellSearchData.mCollisionColumnLeft == collisionNotCalculated) {
        auto collisionColumn = DetectCollisionLeft(piece);
        
        for (auto column = piece.mPosition.x; column >= collisionColumn; --column) {
            auto& cellSearchDataToTheLeft =
                GetSearchDataForOneRotation({column, piece.mPosition.y}, piece.mRotation);
            
            cellSearchDataToTheLeft.mCollisionColumnLeft = collisionColumn;
        }
    }
    
    return cellSearchData.mCollisionColumnLeft;
}

int ValidMovesSearch::HandleCollisionRight(const MovingPiece& piece) {
    auto& cellSearchData = GetSearchDataForOneRotation(piece);
    if (cellSearchData.mCollisionColumnRight == collisionNotCalculated) {
        auto collisionColumn = DetectCollisionRight(piece);
        
        for (auto column = piece.mPosition.x; column <= collisionColumn; ++column) {
            auto& cellSearchDataToTheRight =
                GetSearchDataForOneRotation({column, piece.mPosition.y}, piece.mRotation);
            
            cellSearchDataToTheRight.mCollisionColumnRight = collisionColumn;
        }
    }
    
    return cellSearchData.mCollisionColumnRight;
}

int ValidMovesSearch::HandleCollisionDown(const MovingPiece& piece, bool fillValidArea) {
    auto& cellSearchData = GetSearchDataForOneRotation(piece);
    if (cellSearchData.mCollisionRow == collisionNotCalculated) {
        auto collisionRow = DetectCollisionDown(piece, fillValidArea);
        
        for (auto row = piece.mPosition.y; row >= collisionRow; --row) {
            auto& downwardCellSearchData =
                GetSearchDataForOneRotation({piece.mPosition.x, row}, piece.mRotation);
            
            downwardCellSearchData.mCollisionRow = collisionRow;
        }
    }
    
    return cellSearchData.mCollisionRow;
}

int ValidMovesSearch::DetectCollisionLeft(const MovingPiece& piece) {
    auto pieceBlocks = CreatePieceBlocks(piece);
    return mField.DetectCollisionLeft(pieceBlocks, piece.mPosition, &mValidArea);
}

int ValidMovesSearch::DetectCollisionRight(const MovingPiece& piece) {
    auto pieceBlocks = CreatePieceBlocks(piece);
    return mField.DetectCollisionRight(pieceBlocks, piece.mPosition, &mValidArea);
}

int ValidMovesSearch::DetectCollisionDown(const MovingPiece& piece, bool fillValidArea) {
    auto pieceBlocks = CreatePieceBlocks(piece);
    return mField.DetectCollisionDown(pieceBlocks,
                                      piece.mPosition,
                                      fillValidArea ? &mValidArea : nullptr);
}
