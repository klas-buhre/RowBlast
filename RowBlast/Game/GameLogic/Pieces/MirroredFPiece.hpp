#ifndef MirroredFPiece_hpp
#define MirroredFPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class MirroredFPiece: public Piece {
    public:
        MirroredFPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
