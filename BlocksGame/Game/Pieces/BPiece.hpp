#ifndef BPiece_hpp
#define BPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class BPiece: public Piece {
    public:
        BPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
