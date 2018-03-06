#ifndef RowBomb_hpp
#define RowBomb_hpp

// Game includes.
#include "Piece.hpp"

namespace BlocksGame {
    class RowBomb: public Piece {
    public:
        RowBomb();
        
        bool IsRowBomb() const override;
        bool CanRotateAroundZ() const override;
        int GetNumEmptyTopRows() const override;
    };
}

#endif