#ifndef BigTrianglePiece_hpp
#define BigTrianglePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class BigTrianglePiece: public Piece {
    public:
        BigTrianglePiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
