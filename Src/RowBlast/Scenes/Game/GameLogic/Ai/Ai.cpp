#include "Ai.hpp"

// Game includes.
#include "FallingPiece.hpp"
#include "DraggedPiece.hpp"

using namespace RowBlast;

namespace {
    bool CompareMoves(const Move* a, const Move* b) {
        return a->mScore > b->mScore;
    }
    
    const Level::TutorialMove*
    FindTutorialMove(const std::vector<Level::TutorialMove>& tutorialMoves, const Move& move) {
        for (auto& tutorialMove: tutorialMoves) {
            if (tutorialMove.mPosition == move.mPosition &&
                tutorialMove.mRotation == move.mRotation) {
                
                return &tutorialMove;
            }
        }
        
        return nullptr;
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
    
    auto* predeterminedMove = GetPredeterminedMove(fallingPiece, movesUsed);
    auto* suggestedMoves = GetSuggestedMoves(fallingPiece, movesUsed);
    
    mValidMovesSearch.FindValidMoves(mValidMoves, piece, predeterminedMove, suggestedMoves);
    EvaluateMoves(fallingPiece, movesUsed);
    SortMoves();
    
    return mSortedMoves;
}

const Level::TutorialMove* Ai::GetPredeterminedMove(const FallingPiece& fallingPiece,
                                                    int movesUsed) {
    auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
    if (predeterminedMoves.empty()) {
        return nullptr;
    }

    if (movesUsed < predeterminedMoves.size()) {
        auto& predeterminedMove = predeterminedMoves[movesUsed];
        if (&predeterminedMove.mPieceType == &fallingPiece.GetPieceType()) {
            return &predeterminedMove;
        }
    }

    return nullptr;
}

const std::vector<Level::TutorialMove>* Ai::GetSuggestedMoves(const FallingPiece& fallingPiece,
                                                              int movesUsed) {
    auto& allSuggestedMoves = mLevel->GetSuggestedMoves();
    if (allSuggestedMoves.empty()) {
        return nullptr;
    }

    if (movesUsed < allSuggestedMoves.size()) {
        auto& suggestedMoves = allSuggestedMoves[movesUsed];
        if (!suggestedMoves.empty() &&
            &suggestedMoves.front().mPieceType == &fallingPiece.GetPieceType()) {
            
            return &suggestedMoves;
        }
    }
    
    return nullptr;
}

void Ai::EvaluateMoves(const FallingPiece& fallingPiece, int movesUsed) {
    auto& pieceType = fallingPiece.GetPieceType();
    auto pieceId = fallingPiece.GetId();
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumcolumns = pieceType.GetGridNumColumns();
    auto& moves = mValidMoves.mMoves;
    auto numMoves = moves.Size();
    auto* suggestedTutorialMoves = GetSuggestedMoves(fallingPiece, movesUsed);
    
    for (auto i = 0; i < numMoves; ++i) {
        auto& move = moves.At(i);
        
        auto* suggestedTutorialMove =
            suggestedTutorialMoves ? FindTutorialMove(*suggestedTutorialMoves, move) : nullptr;
        
        if (suggestedTutorialMove) {
            assert(suggestedTutorialMove->mScore.HasValue());
            move.mScore = suggestedTutorialMove->mScore.GetValue();
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
    auto landingHeight =
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y;

    auto filledRowsResult = mField.MarkFilledRowsAndCountGrayLevelCellsInFilledRows();
    auto numFilledRows = filledRowsResult.mFilledRowIndices.Size();
    auto burriedHolesArea = CalculateBurriedHolesArea(numFilledRows, fallingPiece);
    auto wellsArea = mFieldAnalyzer.CalculateWellsAreaInVisibleRows();
    auto numTransitions = static_cast<float>(mFieldAnalyzer.CalculateNumTransitionsInVisibleRows());
    
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
    auto landingHeight =
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y;

    auto filledRowsResult =
        mField.MarkFilledRowsAndCountPieceCellsInFilledRows(fallingPiece.GetId());
    
    auto numFilledRows = filledRowsResult.mFilledRowIndices.Size();
    auto burriedHolesArea = CalculateBurriedHolesArea(numFilledRows, fallingPiece);
    auto wellsArea = mFieldAnalyzer.CalculateWellsAreaInVisibleRows();
    auto numTransitions = static_cast<float>(mFieldAnalyzer.CalculateNumTransitionsInVisibleRows());
    
    move.mScore = -landingHeight
                  + 2.0f * numFilledRows
                  + filledRowsResult.mPieceCellsInFilledRows
                  - 4.1f * burriedHolesArea
                  - 0.6f * wellsArea
                  - 0.6f * numTransitions;
    
    mField.UnmarkFilledRows(filledRowsResult.mFilledRowIndices);
}

void Ai::EvaluateMoveForBuildObjective(Move& move, const FallingPiece& fallingPiece) {
    auto landingHeight =
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        fallingPiece.GetPieceType().GetCenterPosition(move.mRotation).y;
    
    auto numCellsAccordingToBlueprint =
        static_cast<float>(mFieldAnalyzer.CalculateNumCellsAccordingToBlueprintInVisibleRows());

    auto buildHolesArea = mFieldAnalyzer.CalculateBuildHolesAreaInVisibleRows();
    auto buildWellsArea = mFieldAnalyzer.CalculateBuildWellsAreaInVisibleRows();
    
    move.mScore = -landingHeight
                  + 2.0f * numCellsAccordingToBlueprint
                  - 4.0f * buildHolesArea
                  - 0.25f * buildWellsArea;
}

void Ai::SortMoves() {
    mSortedMoves.Clear();
    
    auto& moves = mValidMoves.mMoves;
    auto numMoves = moves.Size();
    
    for (auto i = 0; i < numMoves; ++i) {
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
    
    auto* predeterminedMove = GetPredeterminedMove(fallingPiece, movesUsed);
    auto* suggestedMoves = GetSuggestedMoves(fallingPiece, movesUsed);
    
    mValidMovesSearch.FindValidMoves(mUpdatedValidMoves, piece, predeterminedMove, suggestedMoves);
    return mUpdatedValidMoves;
}

void Ai::CalculateValidArea(const FallingPiece& fallingPiece) {
    mValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    mValidMovesSearch.FindValidMoves(mValidMoves, piece, nullptr, nullptr);
}

bool Ai::IsPieceInValidArea(const DraggedPiece& draggedPiece) {
    MovingPiece piece {
        draggedPiece.GetFieldGridPosition(),
        draggedPiece.GetRotation(),
        draggedPiece.GetPieceType()
    };

    return mValidMovesSearch.IsLocationVisited(piece);
}
