#ifndef SevenPiece_hpp
#define SevenPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class SevenPiece: public Piece {
    public:
        SevenPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
