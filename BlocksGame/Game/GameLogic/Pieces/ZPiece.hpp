#ifndef ZPiece_hpp
#define ZPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class ZPiece: public Piece {
    public:
        ZPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
