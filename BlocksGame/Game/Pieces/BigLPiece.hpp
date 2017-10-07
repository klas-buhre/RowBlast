#ifndef BigLPiece_hpp
#define BigLPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class BigLPiece: public Piece {
    public:
        BigLPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
