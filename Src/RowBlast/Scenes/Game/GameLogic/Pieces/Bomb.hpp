#ifndef Bomb_hpp
#define Bomb_hpp

// Game includes.
#include "Piece.hpp"

namespace RowBlast {
    class Bomb: public Piece {
    public:
        Bomb();
        
        bool IsBomb() const override;
        bool PositionCanBeAdjusteInMovesSearch() const override;
    };
}

#endif
