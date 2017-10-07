#ifndef Bomb_hpp
#define Bomb_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class Bomb: public Piece {
    public:
        Bomb(Pht::IEngine& engine, const GameScene& scene);
        
        bool IsBomb() const override;
        bool CanRotateAroundZ() const override;
        int GetNumEmptyTopRows() const override;
    };
}

#endif
