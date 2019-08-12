#ifndef Ai_hpp
#define Ai_hpp

// Game includes.
#include "ValidMovesSearch.hpp"
#include "FieldAnalyzer.hpp"
#include "Field.hpp"
#include "Level.hpp"

namespace RowBlast {
    class FallingPiece;
    class DraggedPiece;
    class Level;
    
    class Ai {
    public:
        using MovePtrs = Pht::StaticVector<Move*, Field::maxNumColumns * Field::maxNumRows * 4>;
        
        explicit Ai(Field& field);
        
        void Init(const Level& level);
        MovePtrs& CalculateMoves(const FallingPiece& fallingPiece, int movesUsed);
        ValidMoves& FindValidMoves(const FallingPiece& fallingPiece, int movesUsed);
        
    private:
        const Level::TutorialMove* GetPredeterminedMove(const FallingPiece& fallingPiece,
                                                        int movesUsed);
        const std::vector<Level::TutorialMove>* GetSuggestedMoves(const FallingPiece& fallingPiece,
                                                                  int movesUsed);
        void EvaluateMoves(const FallingPiece& fallingPiece, int movesUsed);
        void EvaluateMove(Move& move, const FallingPiece& fallingPiece);
        void EvaluateMoveForClearObjective(Move& move, const FallingPiece& fallingPiece);
        void EvaluateMoveForAsteroidObjective(Move& move, const FallingPiece& fallingPiece);
        void EvaluateMoveForBuildObjective(Move& move, const FallingPiece& fallingPiece);
        float CalculateBurriedHolesArea(int numFilledRows, const FallingPiece& fallingPiece);
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
