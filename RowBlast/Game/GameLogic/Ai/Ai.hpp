#ifndef Ai_hpp
#define Ai_hpp

// Game includes.
#include "ValidMovesSearch.hpp"
#include "FieldAnalyzer.hpp"
#include "Field.hpp"
#include "Level.hpp"

namespace RowBlast {
    class FallingPiece;
    class Level;
    
    class Ai {
    public:
        using MovePtrs = Pht::StaticVector<Move*, Field::maxNumColumns * Field::maxNumRows * 4>;
        
        explicit Ai(Field& field);
        
        void Init(const Level& level);
        MovePtrs& CalculateMoves(const FallingPiece& fallingPiece, int movesUsed);
        ValidMoves& FindValidMoves(const FallingPiece& fallingPiece, int movesUsed);
        
    private:
        const Level::PredeterminedMove* GetPredeterminedMove(int movesUsed);
        void EvaluateMoves(const FallingPiece& fallingPiece);
        void EvaluateMove(Move& move, const FallingPiece& fallingPiece);
        void EvaluateMoveForClearObjective(Move& move, const FallingPiece& fallingPiece);
        void EvaluateMoveForBuildObjective(Move& move, const FallingPiece& fallingPiece);
        void SortMoves();
        
        Field& mField;
        FieldAnalyzer mFieldAnalyzer;
        ValidMovesSearch mValidMovesSearch;
        const Level* mLevel {nullptr};
        ValidMoves mValidMoves;
        ValidMoves mUpdatedValidMoves;
        MovePtrs mSortedMoves;
    };
}

#endif
