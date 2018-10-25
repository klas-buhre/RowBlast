#ifndef PlusPiece_hpp
#define PlusPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class PlusPiece: public Piece {
    public:
        PlusPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
