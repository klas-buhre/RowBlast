#ifndef ValidAreaAnimation_hpp
#define ValidAreaAnimation_hpp

#include <vector>

// Game includes.
#include "MoveDefinitions.hpp"

namespace RowBlast {
    class Field;
    class Piece;
    
    class ValidAreaAnimation {
    public:
        ValidAreaAnimation(Field& field);
        
        void Init();
        void Start(const Moves& allValidMoves, const Piece& pieceType, Rotation rotation);
        void Stop();
        bool IsActive() const;
        bool IsCellValid(int row, int column) const;
        
    private:
        void FillValidAreaAboveMove(const Move& move, const Piece& pieceType);
        void ClearGrid();
        
        enum class State {
            Active,
            Inactive
        };
        
        using Grid = std::vector<std::vector<int>>;
        
        Field& mField;
        State mState;
        Grid mGrid;
    };
}

#endif
