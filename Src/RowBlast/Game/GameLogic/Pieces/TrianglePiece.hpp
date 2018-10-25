#ifndef TrianglePiece_hpp
#define TrianglePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class TrianglePiece: public Piece {
    public:
        TrianglePiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
