#ifndef RowBomb_hpp
#define RowBomb_hpp

// Game includes.
#include "Piece.hpp"

namespace RowBlast {
    class RowBomb: public Piece {
    public:
        RowBomb();
        
        bool IsRowBomb() const override;
        bool CanRotateAroundZ() const override;
        int GetNumEmptyTopRows() const override;
        bool PositionCanBeAdjusteInMovesSearch() const override;
    };
}

#endif
