#ifndef DPiece_hpp
#define DPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class DPiece: public Piece {
    public:
        DPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
