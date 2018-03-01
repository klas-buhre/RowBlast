#include "Ai.hpp"

// Game includes.
#include "FallingPiece.hpp"
#include "Level.hpp"

using namespace BlocksGame;

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

Ai::MovePtrs& Ai::CalculateMoves(const FallingPiece& fallingPiece) {
    mValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    mValidMovesSearch.FindValidMoves(mValidMoves, piece);
    EvaluateMoves(fallingPiece);
    SortMoves();
    
    return mSortedMoves;
}

void Ai::EvaluateMoves(const FallingPiece& fallingPiece) {
    auto& pieceType {fallingPiece.GetPieceType()};
    auto pieceId {fallingPiece.GetId()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumcolumns {pieceType.GetGridNumColumns()};
    auto& moves {mValidMoves.mMoves};
    auto numMoves {moves.Size()};
    
    for (auto i {0}; i < numMoves; ++i) {
        auto& move {moves.At(i)};
        PieceBlocks pieceBlocks {pieceType.GetGrid(move.mRotation), pieceNumRows, pieceNumcolumns};
        
        mField.LandPieceBlocks(pieceBlocks, pieceId, move.mPosition, false, false);
        EvaluateMove(move, fallingPiece);
        mField.RemovePiece(pieceId, move.mPosition, pieceNumRows, pieceNumcolumns);
    }
}

void Ai::EvaluateMove(Move& move, const FallingPiece& fallingPiece) {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            EvaluateMoveForClearObjective(move, fallingPiece);
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

    auto filledRowsResult {
        mField.MarkFilledRowsAndCountPieceCellsInFilledRows(fallingPiece.GetId())
    };
    
    auto numFilledRows {filledRowsResult.mFilledRowIndices.Size()};
    auto filledRowsMetric {0.0f};
    
    if (numFilledRows > 0) {
        filledRowsMetric = static_cast<float>(numFilledRows) * filledRowsResult.mPieceCellsInFilledRows;
    }
    
    auto burriedHolesArea {mFieldAnalyzer.GetBurriedHolesAreaInVisibleRows()};
    auto wellsArea {mFieldAnalyzer.GetWellsAreaInVisibleRows()};
    auto numTransitions {static_cast<float>(mFieldAnalyzer.GetNumTransitionsInVisibleRows())};
    
    move.mScore = -landingHeight
                  + filledRowsMetric
                  - 4.0f * burriedHolesArea
                  - wellsArea
                  - numTransitions;
    
    mField.UnmarkFilledRows(filledRowsResult.mFilledRowIndices);
}

void Ai::EvaluateMoveForBuildObjective(Move& move, const FallingPiece& fallingPiece) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y
    };
    
    auto numCellsAccordingToBlueprint {
        static_cast<float>(mFieldAnalyzer.GetNumCellsAccordingToBlueprintInVisibleRows())
    };

    auto buildHolesArea {mFieldAnalyzer.GetBuildHolesAreaInVisibleRows()};
    auto buildWellsArea {mFieldAnalyzer.GetBuildWellsAreaInVisibleRows()};
    
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

ValidMoves& Ai::FindValidMoves(const FallingPiece& fallingPiece) {
    mUpdatedValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    mValidMovesSearch.FindValidMoves(mUpdatedValidMoves, piece);
    return mUpdatedValidMoves;
}