#include "Ai.hpp"

// Game includes.
#include "FallingPiece.hpp"

using namespace RowBlast;

namespace {
    bool CompareMoves(const Move* a, const Move* b) {
        return a->mScore > b->mScore;
    }
}

Ai::Ai(Field& field) :
    mField {field},
    mFieldAnalyzer {field},
    mValidMovesSearch {field} {}

void Ai::Init(const Level& level) {
    mLevel = &level;
    mValidMovesSearch.Init();
}

Ai::MovePtrs& Ai::CalculateMoves(const FallingPiece& fallingPiece, int movesUsed) {
    mValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    auto* predeterminedMove {GetPredeterminedMove(fallingPiece, movesUsed)};
    auto* suggestedMove {GetSuggestedMove(fallingPiece, movesUsed)};
    
    mValidMovesSearch.FindValidMoves(mValidMoves, piece, predeterminedMove, suggestedMove);
    EvaluateMoves(fallingPiece, movesUsed);
    SortMoves();
    
    return mSortedMoves;
}

const Level::TutorialMove* Ai::GetPredeterminedMove(const FallingPiece& fallingPiece,
                                                    int movesUsed) {
    auto& predeterminedMoves {mLevel->GetPredeterminedMoves()};
    
    if (predeterminedMoves.empty()) {
        return nullptr;
    }

    if (movesUsed < predeterminedMoves.size()) {
        auto& predeterminedMove {predeterminedMoves[movesUsed]};
        
        if (&predeterminedMove.mPieceType == &fallingPiece.GetPieceType()) {
            return &predeterminedMove;
        }
    }

    return nullptr;
}

const Level::TutorialMove* Ai::GetSuggestedMove(const FallingPiece& fallingPiece, int movesUsed) {
    auto& suggestedMoves {mLevel->GetSuggestedMoves()};
    
    if (suggestedMoves.empty()) {
        return nullptr;
    }

    if (movesUsed < suggestedMoves.size()) {
        auto& suggestedMove {suggestedMoves[movesUsed]};
        
        if (&suggestedMove.mPieceType == &fallingPiece.GetPieceType()) {
            return &suggestedMove;
        }
    }
    
    return nullptr;
}

void Ai::EvaluateMoves(const FallingPiece& fallingPiece, int movesUsed) {
    auto& pieceType {fallingPiece.GetPieceType()};
    auto pieceId {fallingPiece.GetId()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumcolumns {pieceType.GetGridNumColumns()};
    auto& moves {mValidMoves.mMoves};
    auto numMoves {moves.Size()};
    auto* suggestedMove {GetSuggestedMove(fallingPiece, movesUsed)};
    
    for (auto i {0}; i < numMoves; ++i) {
        auto& move {moves.At(i)};
        
        if (suggestedMove && suggestedMove->mPosition == move.mPosition &&
            suggestedMove->mRotation == move.mRotation) {
            
            move.mScore = std::numeric_limits<float>::max();
        } else {
            PieceBlocks pieceBlocks {
                pieceType.GetGrid(move.mRotation),
                pieceNumRows,
                pieceNumcolumns
            };
            
            mField.LandPieceBlocks(pieceBlocks, pieceId, move.mPosition, false, false, false);
            EvaluateMove(move, fallingPiece);
            mField.RemovePiece(pieceId, move.mPosition, pieceNumRows, pieceNumcolumns);
        }
    }
}

void Ai::EvaluateMove(Move& move, const FallingPiece& fallingPiece) {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            EvaluateMoveForClearObjective(move, fallingPiece);
            break;
        case Level::Objective::BringDownTheAsteroid:
            EvaluateMoveForAsteroidObjective(move, fallingPiece);
            break;
        case Level::Objective::Build:
            EvaluateMoveForBuildObjective(move, fallingPiece);
            break;
    }
}

void Ai::EvaluateMoveForClearObjective(Move& move, const FallingPiece& fallingPiece) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y
    };

    auto filledRowsResult {mField.MarkFilledRowsAndCountGrayLevelCellsInFilledRows()};
    auto numFilledRows {filledRowsResult.mFilledRowIndices.Size()};
    auto burriedHolesArea {CalculateBurriedHolesArea(numFilledRows, fallingPiece)};
    auto wellsArea {mFieldAnalyzer.CalculateWellsAreaInVisibleRows()};
    auto numTransitions {static_cast<float>(mFieldAnalyzer.CalculateNumTransitionsInVisibleRows())};
    
    move.mScore = -landingHeight
                  + 2.0f * numFilledRows
                  + filledRowsResult.mGrayLevelCellsInFilledRows
                  - 4.1f * burriedHolesArea
                  - 0.6f * wellsArea
                  - 0.6f * numTransitions;

    mField.UnmarkFilledRows(filledRowsResult.mFilledRowIndices);
}

float Ai::CalculateBurriedHolesArea(int numFilledRows, const FallingPiece& fallingPiece) {
    if (numFilledRows == 0) {
        return mFieldAnalyzer.CalculateBurriedHolesAreaInVisibleRows();
    }
    
    return mFieldAnalyzer.CalculateBurriedHolesAreaInVisibleRowsWithGravity(fallingPiece.GetId());
}

void Ai::EvaluateMoveForAsteroidObjective(Move& move, const FallingPiece& fallingPiece) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y
    };

    auto filledRowsResult {
        mField.MarkFilledRowsAndCountPieceCellsInFilledRows(fallingPiece.GetId())
    };
    
    auto numFilledRows {filledRowsResult.mFilledRowIndices.Size()};
    auto burriedHolesArea {CalculateBurriedHolesArea(numFilledRows, fallingPiece)};
    auto wellsArea {mFieldAnalyzer.CalculateWellsAreaInVisibleRows()};
    auto numTransitions {static_cast<float>(mFieldAnalyzer.CalculateNumTransitionsInVisibleRows())};
    
    move.mScore = -landingHeight
                  + 2.0f * numFilledRows
                  + filledRowsResult.mPieceCellsInFilledRows
                  - 4.1f * burriedHolesArea
                  - 0.6f * wellsArea
                  - 0.6f * numTransitions;
    
    mField.UnmarkFilledRows(filledRowsResult.mFilledRowIndices);
}

void Ai::EvaluateMoveForBuildObjective(Move& move, const FallingPiece& fallingPiece) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y
    };
    
    auto numCellsAccordingToBlueprint {
        static_cast<float>(mFieldAnalyzer.CalculateNumCellsAccordingToBlueprintInVisibleRows())
    };

    auto buildHolesArea {mFieldAnalyzer.CalculateBuildHolesAreaInVisibleRows()};
    auto buildWellsArea {mFieldAnalyzer.CalculateBuildWellsAreaInVisibleRows()};
    
    move.mScore = -landingHeight
                  + 2.0f * numCellsAccordingToBlueprint
                  - 4.0f * buildHolesArea
                  - 0.25f * buildWellsArea;
}

void Ai::SortMoves() {
    mSortedMoves.Clear();
    
    auto& moves {mValidMoves.mMoves};
    auto numMoves {moves.Size()};
    
    for (auto i {0}; i < numMoves; ++i) {
        mSortedMoves.PushBack(&moves.At(i));
    }

    mSortedMoves.Sort(CompareMoves);
}

ValidMoves& Ai::FindValidMoves(const FallingPiece& fallingPiece, int movesUsed) {
    mUpdatedValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    auto* predeterminedMove {GetPredeterminedMove(fallingPiece, movesUsed)};
    auto* suggestedMove {GetSuggestedMove(fallingPiece, movesUsed)};
    
    mValidMovesSearch.FindValidMoves(mUpdatedValidMoves, piece, predeterminedMove, suggestedMove);
    return mUpdatedValidMoves;
}
