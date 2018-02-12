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
}

Movement::Movement(const Pht::Vec2& position, Rotation rotation, const Movement* previous) :
    mPosition {position},
    mRotation {rotation},
    mPrevious {previous} {}

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
        piece.mRotation = static_cast<Rotation>(rotation);
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

void ValidMovesSearch::InitSearchGrid() {
    enum class OverhangScanState {
        FreeSpace,
        OccupiedSpace
    };
    
    for (auto column {0}; column < mField.GetNumColumns(); ++column) {
        auto overhangScanState {OverhangScanState::FreeSpace};
        
        for (auto row {mField.GetNumRows() - 1}; row >= 0; --row) {
            auto& cellSearchData {mSearchGrid[row][column]};
            auto& visitedArray {cellSearchData.mIsVisited};
            
            for (auto& isVisited: visitedArray) {
                isVisited = false;
            }
            
            auto& foundMovesArray {cellSearchData.mFoundMoves};
            
            for (auto& foundMove: foundMovesArray) {
                foundMove = nullptr;
            }
            
            cellSearchData.mUnderOverhangTip = false;
            
            auto fieldCellIsEmpty {mField.GetCell(row, column).IsEmpty()};
            auto fieldCellIsNotFull {!mField.GetCell(row, column).IsFull()};
            
            switch (overhangScanState) {
                case OverhangScanState::FreeSpace:
                    if (!fieldCellIsEmpty) {
                        overhangScanState = OverhangScanState::OccupiedSpace;
                    }
                    break;
                case OverhangScanState::OccupiedSpace:
                    if (fieldCellIsNotFull) {
                        if (column - 1 >= 0) {
                            if (mField.GetCell(row + 1, column - 1).IsEmpty()) {
                                cellSearchData.mUnderOverhangTip = true;
                            }
                        }
                        
                        if (column + 1 < mField.GetNumColumns()) {
                            if (mField.GetCell(row + 1, column + 1).IsEmpty()) {
                                cellSearchData.mUnderOverhangTip = true;
                            }
                        }
                        
                        overhangScanState = OverhangScanState::FreeSpace;
                    }
                    break;
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
    
    for (auto rotation {0}; rotation < numRotations; ++rotation) {
        piece.mRotation = static_cast<Rotation>(rotation);
        
        if (positionAdjustment == PositionAdjustment::Yes) {
            AdjustPosition(piece);
        }
        
        if (IsVisited(piece)) {
            continue;
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
        
        MarkAsVisited(piece);
    }
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
    auto xEnd {DetectCollisionLeft(piece)};
    
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
    auto xEnd {DetectCollisionRight(piece)};
    
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
    auto yEnd {DetectCollisionDown(piece)};
    
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

bool ValidMovesSearch::IsVisited(const MovingPiece& piece) const {
    auto gridPosition {CalculateSearchGridPosition(piece)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mIsVisited[static_cast<int>(piece.mRotation)];
}

void ValidMovesSearch::MarkAsVisited(const MovingPiece& piece) {
    auto gridPosition {CalculateSearchGridPosition(piece)};
    mSearchGrid[gridPosition.y][gridPosition.x].mIsVisited[static_cast<int>(piece.mRotation)] = true;
}

Move* ValidMovesSearch::GetFoundMove(const MovingPiece& piece) const {
    auto gridPosition {CalculateSearchGridPosition(piece)};
    return mSearchGrid[gridPosition.y][gridPosition.x].mFoundMoves[static_cast<int>(piece.mRotation)];
}

void ValidMovesSearch::SetFoundMove(const MovingPiece& piece, Move& move) {
    auto gridPosition {CalculateSearchGridPosition(piece)};
    mSearchGrid[gridPosition.y][gridPosition.x].mFoundMoves[static_cast<int>(piece.mRotation)] = &move;
}

Pht::IVec2 ValidMovesSearch::CalculateSearchGridPosition(const MovingPiece& piece) const {
    auto& piecePosition {piece.mPosition};
    
    return Pht::IVec2 {
        piecePosition.x >= 0 ? piecePosition.x : piecePosition.x + mField.GetNumColumns(),
        piecePosition.y >= 0 ? piecePosition.y : piecePosition.y + mField.GetNumRows()
    };
}

bool ValidMovesSearch::IsCollision(const MovingPiece& piece) const {
    const auto& position {piece.mPosition};
    auto pieceBlocks {CreatePieceBlocks(piece)};
    
    mField.CheckCollision(mCollisionResult, pieceBlocks, position, Pht::IVec2{0, 0}, false);
    
    return mCollisionResult.mIsCollision == IsCollision::Yes;
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
