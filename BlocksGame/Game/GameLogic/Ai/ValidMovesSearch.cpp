#include "ValidMovesSearch.hpp"

#include <assert.h>

// Game includes.
#include "CollisionDetection.hpp"

using namespace BlocksGame;

namespace {
    const auto maxRotateAdjustment {2};
    
    const Movement* AddMovement(ValidMoves& validMoves,
                                const MovingPiece& piece,
                                const Movement* previousMovement) {
        auto& position {piece.mPosition};
        
        Pht::Vec2 floatPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
        Movement movement {floatPosition, piece.mRotation, previousMovement};
        validMoves.mMovements.PushBack(movement);
        return &validMoves.mMovements.Back();
    }
    
    int CalcNumMovements(const Movement& finalMovement) {
        auto numMovements {0};
        
        for (auto* movement {&finalMovement}; movement; movement = movement->GetPrevious()) {
            ++numMovements;
        }
        
        return numMovements;
    }
    
    PieceBlocks CreatePieceBlocks(const MovingPiece& piece) {
        auto& pieceType {piece.mPieceType};
        
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

Movement::Movement(const Pht::Vec2& position, Rotation rotation, const Movement* previous) :
    mPosition {position},
    mRotation {rotation},
    mPrevious {previous} {}

void MovingPiece::RotateClockwise() {
    auto numRotations {mPieceType.GetNumRotations()};
    mRotation = static_cast<Rotation>((static_cast<int>(mRotation) + 1) % numRotations);
}

void MovingPiece::RotateAntiClockwise() {
    auto newRotation {static_cast<int>(mRotation) - 1};
    
    if (newRotation < 0) {
        newRotation += mPieceType.GetNumRotations();
    }
    
    mRotation = static_cast<Rotation>(newRotation);
}

void ValidMoves::Clear() {
    mMoves.Clear();
    mMovements.Clear();
}

ValidMovesSearch::ValidMovesSearch(Field& field) :
    mField {field} {}

void ValidMovesSearch::Init() {
    auto numRows {mField.GetNumRows()};
    auto numColumns {mField.GetNumColumns()};
    
    mSearchGrid.clear();
    mSearchGrid.reserve(numRows);
        
    std::vector<CellSearchData> row(numColumns);
        
    for (auto rowIndex {0}; rowIndex < numRows; ++rowIndex) {
        mSearchGrid.push_back(row);
    }
}

void ValidMovesSearch::FindValidMoves(ValidMoves& validMoves, MovingPiece piece) {
    InitSearchGrid();
    FindMostValidMoves(validMoves, piece);
    ResetVisitedLocations();
    FindAllValidMoves(validMoves, piece);
}

void ValidMovesSearch::InitSearchGrid() {
    auto numRows {mField.GetNumRows()};
    auto numColumns {mField.GetNumColumns()};

    enum class OverhangScanState {
        FreeSpace,
        OccupiedSpace
    };
    
    for (auto column {0}; column < numColumns; ++column) {
        auto overhangScanState {OverhangScanState::FreeSpace};
        
        for (auto row {numRows - 1}; row >= 0; --row) {
            auto& cellSearchData {mSearchGrid[row][column]};
            auto& searchDataForRotations {cellSearchData.mData};
            
            for (auto& searchData: searchDataForRotations) {
                searchData = SearchDataForOneRotation {};
            }

            cellSearchData.mUnderOverhangTip = false;
            
            auto& fieldCell {mField.GetCell(row, column)};
            
            switch (overhangScanState) {
                case OverhangScanState::FreeSpace:
                    if (!fieldCell.IsEmpty()) {
                        auto& firstSubCell {fieldCell.mFirstSubCell};
                        auto& secondSubCell {fieldCell.mSecondSubCell};
                        auto leftColumn {column - 1};
                        auto rightColumn {column + 1};
                        
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
                        auto rowAbove {row + 1};
                        
                        if (rowAbove < numRows) {
                            auto leftColumn {column - 1};
                            
                            if (leftColumn >= 0 && mField.GetCell(rowAbove, leftColumn).IsEmpty() &&
                                !CellFillsLeftSide(fieldCell) &&
                                !CellFillsRightSide(mField.GetCell(row, leftColumn))) {
                                cellSearchData.mUnderOverhangTip = true;
                            }
                            
                            auto rightColumn {column + 1};
                            
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

void ValidMovesSearch::ResetVisitedLocations() {
    auto numRows {mField.GetNumRows()};
    auto numColumns {mField.GetNumColumns()};

    for (auto row {0}; row < numRows; ++row) {
        for (auto column {0}; column < numColumns; ++column) {
            auto& searchDataForRotations {mSearchGrid[row][column].mData};
            
            for (auto& searchData: searchDataForRotations) {
                searchData.mIsVisited = false;
            }
        }
    }
}

void ValidMovesSearch::AdjustPosition(MovingPiece& piece) {
    const auto& position {piece.mPosition};
    auto pieceBlocks {CreatePieceBlocks(piece)};
    
    mField.CheckCollision(mCollisionResult, pieceBlocks, position, Pht::IVec2{0, 0}, false);
    
    if (mCollisionResult.mIsCollision == IsCollision::Yes) {
        auto collisionDirection {
            CollisionDetection::CalculateCollisionDirection(mCollisionResult.mCollisionPoints,
                                                            pieceBlocks,
                                                            piece.mRotation,
                                                            position,
                                                            mField)
        };
        
        switch (collisionDirection) {
            case Direction::Right: {
                auto freeXPos {mField.DetectFreeSpaceLeft(pieceBlocks, position)};
                if (position.x - freeXPos <= maxRotateAdjustment) {
                    piece.mPosition.x = freeXPos;
                }
                break;
            }
            case Direction::Left: {
                auto freeXPos {mField.DetectFreeSpaceRight(pieceBlocks, position)};
                if (freeXPos - position.x <= maxRotateAdjustment) {
                    piece.mPosition.x = freeXPos;
                }
                break;
            }
            case Direction::Up: {
                auto freeYPos {mField.DetectFreeSpaceDown(pieceBlocks, position)};
                if (position.y - freeYPos <= maxRotateAdjustment) {
                    piece.mPosition.y = freeYPos;
                }
                break;
            }
            case Direction::Down: {
                auto freeYPos {mField.DetectFreeSpaceUp(pieceBlocks, position)};
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

void ValidMovesSearch::FindMostValidMoves(ValidMoves& validMoves, MovingPiece piece) {
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
    
    auto numRotations {piece.mPieceType.GetNumRotations()};
    
    for (auto rotation {0}; rotation < numRotations; ++rotation) {
        // TODO: Try anti-clockwise also?
        piece.RotateClockwise();
        AdjustPosition(piece);
        
        if (IsCollision(piece)) {
            continue;
        }
        
        auto* rotationMovement {AddMovement(validMoves, piece, nullptr)};
        
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
    
    auto numRotations {piece.mPieceType.GetNumRotations()};
    auto rotationAtEntry {piece.mRotation};
    
    for (auto rotation {0}; rotation < numRotations; ++rotation) {
        piece.RotateClockwise();
        
        if (!FindValidMovesForRotation(validMoves,
                                       piece,
                                       searchDirection,
                                       previousMovement,
                                       allowRecursion,
                                       positionAdjustment)) {
            break;
        }
    }
    
    piece.mRotation = rotationAtEntry;
    
    for (auto rotation {0}; rotation < numRotations; ++rotation) {
        piece.RotateAntiClockwise();
        
        if (!FindValidMovesForRotation(validMoves,
                                       piece,
                                       searchDirection,
                                       previousMovement,
                                       allowRecursion,
                                       positionAdjustment)) {
            break;
        }
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

    auto* rotationMovement {AddMovement(validMoves, piece, previousMovement)};

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
    
    auto xBegin {piece.mPosition.x};
    auto xEnd {HandleCollisionLeft(piece)};
    
    for (auto x {xBegin}; x >= xEnd; --x) {
        piece.mPosition.x = x;

        auto* newPositionMovement {AddMovement(validMoves, piece, previousMovement)};
        
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
    
    auto xBegin {piece.mPosition.x};
    auto xEnd {HandleCollisionRight(piece)};
    
    for (auto x {xBegin}; x <= xEnd; ++x) {
        piece.mPosition.x = x;

        auto* newPositionMovement {AddMovement(validMoves, piece, previousMovement)};
        
        SearchDown(validMoves, piece, newPositionMovement, allowRecursion, positionAdjustment);
    }
}

void ValidMovesSearch::SearchDown(ValidMoves& validMoves,
                                  MovingPiece piece,
                                  const Movement* previousMovement,
                                  AllowRecursion allowRecursion,
                                  PositionAdjustment positionAdjustment) {
    
    auto yBegin {piece.mPosition.y};
    auto yEnd {HandleCollisionDown(piece)};
    
    auto pieceTypeRightOverhangCheckPos {
        piece.mPieceType.GetRightOverhangCheckPosition(piece.mRotation)
    };
    
    auto pieceTypeLeftOverhangCheckPos {
        piece.mPieceType.GetLeftOverhangCheckPosition(piece.mRotation)
    };
    
    auto pieceTypeRightExtremityCheckPos {
        piece.mPieceType.GetRightExtremityCheckPosition(piece.mRotation)
    };

    auto pieceTypeLeftExtremityCheckPos {
        piece.mPieceType.GetLeftExtremityCheckPosition(piece.mRotation)
    };
    
    for (auto y {yBegin}; y >= yEnd; --y) {
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
    if (IsDuplicateMoveFoundAtDifferentLocation(piece)) {
        return;
    }
    
    if (auto* foundMove {GetFoundMove(piece)}) {
        auto foundMoveNumMovements {CalcNumMovements(*foundMove->mLastMovement)};
        auto thisMoveNumMovements {CalcNumMovements(*previousMovement) + 1};
        
        if (thisMoveNumMovements < foundMoveNumMovements) {
            auto* lastMovement {AddMovement(validMoves, piece, previousMovement)};
            *foundMove = Move {piece.mPosition, piece.mRotation, lastMovement};
        }
    } else {
        auto* lastMovement {AddMovement(validMoves, piece, previousMovement)};
        Move move {piece.mPosition, piece.mRotation, lastMovement};
    
        validMoves.mMoves.PushBack(move);
        SetFoundMove(piece, validMoves.mMoves.Back());
    }
}

void ValidMovesSearch::FindAllValidMoves(ValidMoves& validMoves, MovingPiece piece) {
    auto* rootMovement {AddMovement(validMoves, piece, nullptr)};
    Search(validMoves, piece, rootMovement, SearchMovement::Start);
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
    
    MarkLocationAsVisited(piece);
    
    switch (HandleCollision(piece, searchMovement)) {
        case SearchCollisionResult::Collision:
            return;
        case SearchCollisionResult::FoundMove:
            SaveMoveIfNotFoundBefore(validMoves, piece, previousMovement);
            break;
        case SearchCollisionResult::NoCollision:
            break;
    }
    
    Search(validMoves, piece, previousMovement, SearchMovement::Down);
    Search(validMoves, piece, previousMovement, SearchMovement::Right);
    Search(validMoves, piece, previousMovement, SearchMovement::Left);
    Search(validMoves, piece, previousMovement, SearchMovement::RotateClockwise);
    Search(validMoves, piece, previousMovement, SearchMovement::RotateAntiClockwise);
}

bool ValidMovesSearch::MovePieceAndCheckEdges(MovingPiece& piece, SearchMovement searchMovement) {
    switch (searchMovement) {
        case SearchMovement::Down: {
            piece.mPosition.y--;
            auto& pieceDimensions {piece.mPieceType.GetDimensions(piece.mRotation)};
            if (piece.mPosition.y + pieceDimensions.mYmin < mField.GetLowestVisibleRow()) {
                return false;
            }
            break;
        }
        case SearchMovement::Right: {
            piece.mPosition.x++;
            auto& pieceDimensions {piece.mPieceType.GetDimensions(piece.mRotation)};
            if (piece.mPosition.x + pieceDimensions.mXmax >= mField.GetNumColumns()) {
                return false;
            }
            break;
        }
        case SearchMovement::Left: {
            piece.mPosition.x--;
            auto& pieceDimensions {piece.mPieceType.GetDimensions(piece.mRotation)};
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
            auto piecePreviousState {piece};
            piecePreviousState.mPosition.y++;
            auto collisionRow {HandleCollisionDown(piecePreviousState)};
            if (collisionRow == piece.mPosition.y) {
                return SearchCollisionResult::FoundMove;
            } else if (collisionRow == piecePreviousState.mPosition.y) {
                return SearchCollisionResult::Collision;
            }
            break;
        }
        case SearchMovement::Right: {
            auto piecePreviousState {piece};
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
            auto piecePreviousState {piece};
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

void ValidMovesSearch::SaveMoveIfNotFoundBefore(ValidMoves& validMoves,
                                                const MovingPiece& piece,
                                                const Movement* previousMovement) {
    if (GetFoundMove(piece) || IsDuplicateMoveFoundAtDifferentLocation(piece)) {
        return;
    }
    
    auto* lastMovement {AddMovement(validMoves, piece, previousMovement)};
    Move move {piece.mPosition, piece.mRotation, lastMovement};

    validMoves.mMoves.PushBack(move);
    SetFoundMove(piece, validMoves.mMoves.Back());
}

bool ValidMovesSearch::IsDuplicateMoveFoundAtDifferentLocation(const MovingPiece& piece) const {
    auto& duplicateMoveCheck {piece.mPieceType.GetDuplicateMoveCheck(piece.mRotation)};
    
    if (duplicateMoveCheck.HasValue()) {
        auto& duplicateMoveCheckValue {duplicateMoveCheck.GetValue()};
        auto checkPosition {piece.mPosition + duplicateMoveCheckValue.mRelativePosition};
        
        auto& searchData {
            GetSearchDataForOneRotation(checkPosition, duplicateMoveCheckValue.mRotation)
        };
        
        if (searchData.mFoundMove) {
            return true;
        }
    }
    
    return false;
}

bool ValidMovesSearch::IsLocationVisited(const MovingPiece& piece) const {
    auto gridPosition {CalculateSearchGridPosition(piece.mPosition)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mIsVisited;
}

void ValidMovesSearch::MarkLocationAsVisited(const MovingPiece& piece) {
    auto gridPosition {CalculateSearchGridPosition(piece.mPosition)};
    mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mIsVisited = true;
}

Move* ValidMovesSearch::GetFoundMove(const MovingPiece& piece) const {
    auto gridPosition {CalculateSearchGridPosition(piece.mPosition)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mFoundMove;
}

void ValidMovesSearch::SetFoundMove(const MovingPiece& piece, Move& move) {
    auto gridPosition {CalculateSearchGridPosition(piece.mPosition)};
    mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)].mFoundMove = &move;
}

ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const MovingPiece& piece) {
    auto gridPosition {CalculateSearchGridPosition(piece.mPosition)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(piece.mRotation)];
}

ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const Pht::IVec2& position, Rotation rotation) {
    auto gridPosition {CalculateSearchGridPosition(position)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(rotation)];
}

const ValidMovesSearch::SearchDataForOneRotation&
ValidMovesSearch::GetSearchDataForOneRotation(const Pht::IVec2& position, Rotation rotation) const {
    auto gridPosition {CalculateSearchGridPosition(position)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mData[static_cast<int>(rotation)];
}

Pht::IVec2 ValidMovesSearch::CalculateSearchGridPosition(const Pht::IVec2& position) const {
    return Pht::IVec2 {
        position.x >= 0 ? position.x : position.x + mField.GetNumColumns(),
        position.y >= 0 ? position.y : position.y + mField.GetNumRows()
    };
}

bool ValidMovesSearch::IsCollision(const MovingPiece& piece) const {
    const auto& position {piece.mPosition};
    auto pieceBlocks {CreatePieceBlocks(piece)};
    
    mField.CheckCollision(mCollisionResult, pieceBlocks, position, Pht::IVec2{0, 0}, false);
    
    return mCollisionResult.mIsCollision == IsCollision::Yes;
}

int ValidMovesSearch::HandleCollisionLeft(const MovingPiece& piece) {
    auto& cellSearchData {GetSearchDataForOneRotation(piece)};
    
    if (cellSearchData.mCollisionColumnLeft == collisionNotCalculated) {
        auto collisionColumn {DetectCollisionLeft(piece)};
        
        for (auto column {piece.mPosition.x}; column >= collisionColumn; --column) {
            auto& cellSearchDataToTheLeft {
                GetSearchDataForOneRotation({column, piece.mPosition.y}, piece.mRotation)
            };
            
            cellSearchDataToTheLeft.mCollisionColumnLeft = collisionColumn;
        }
    }
    
    return cellSearchData.mCollisionColumnLeft;
}

int ValidMovesSearch::HandleCollisionRight(const MovingPiece& piece) {
    auto& cellSearchData {GetSearchDataForOneRotation(piece)};
    
    if (cellSearchData.mCollisionColumnRight == collisionNotCalculated) {
        auto collisionColumn {DetectCollisionRight(piece)};
        
        for (auto column {piece.mPosition.x}; column <= collisionColumn; ++column) {
            auto& cellSearchDataToTheRight {
                GetSearchDataForOneRotation({column, piece.mPosition.y}, piece.mRotation)
            };
            
            cellSearchDataToTheRight.mCollisionColumnRight = collisionColumn;
        }
    }
    
    return cellSearchData.mCollisionColumnRight;
}

int ValidMovesSearch::HandleCollisionDown(const MovingPiece& piece) {
    auto& cellSearchData {GetSearchDataForOneRotation(piece)};
    
    if (cellSearchData.mCollisionRow == collisionNotCalculated) {
        auto collisionRow {DetectCollisionDown(piece)};
        
        for (auto row {piece.mPosition.y}; row >= collisionRow; --row) {
            auto& downwardCellSearchData {
                GetSearchDataForOneRotation({piece.mPosition.x, row}, piece.mRotation)
            };
            
            downwardCellSearchData.mCollisionRow = collisionRow;
        }
    }
    
    return cellSearchData.mCollisionRow;
}

int ValidMovesSearch::DetectCollisionLeft(const MovingPiece& piece) const {
    auto pieceBlocks {CreatePieceBlocks(piece)};
    return mField.DetectCollisionLeft(pieceBlocks, piece.mPosition);
}

int ValidMovesSearch::DetectCollisionRight(const MovingPiece& piece) const {
    auto pieceBlocks {CreatePieceBlocks(piece)};
    return mField.DetectCollisionRight(pieceBlocks, piece.mPosition);
}

int ValidMovesSearch::DetectCollisionDown(const MovingPiece& piece) const {
    auto pieceBlocks {CreatePieceBlocks(piece)};
    return mField.DetectCollisionDown(pieceBlocks, piece.mPosition);
}
