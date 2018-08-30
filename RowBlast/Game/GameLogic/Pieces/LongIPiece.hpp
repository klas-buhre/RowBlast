#ifndef LongIPiece_hpp
#define LongIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class LongIPiece: public Piece {
    public:
        LongIPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
