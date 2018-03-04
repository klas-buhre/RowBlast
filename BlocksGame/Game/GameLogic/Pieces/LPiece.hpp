#ifndef LPiece_hpp
#define LPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class LPiece: public Piece {
    public:
        LPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
