#ifndef RowBomb_hpp
#define RowBomb_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class RowBomb: public Piece {
    public:
        RowBomb(Pht::IEngine& engine, const GameScene& scene);
        
        bool IsBomb() const override;
        bool IsRowBomb() const override;
        bool CanRotateAroundZ() const override;
        int GetNumEmptyTopRows() const override;
    };
}

#endif
