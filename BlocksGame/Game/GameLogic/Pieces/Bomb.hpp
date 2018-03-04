#ifndef Bomb_hpp
#define Bomb_hpp

// Game includes.
#include "Piece.hpp"

namespace BlocksGame {
    class Bomb: public Piece {
    public:
        Bomb();
        
        bool IsBomb() const override;
        bool CanRotateAroundZ() const override;
        int GetNumEmptyTopRows() const override;
    };
}

#endif
