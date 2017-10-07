#ifndef MirroredZPiece_hpp
#define MirroredZPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class MirroredZPiece: public Piece {
    public:
        MirroredZPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
