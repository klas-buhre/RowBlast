#ifndef PyramidPiece_hpp
#define PyramidPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class PyramidPiece: public Piece {
    public:
        PyramidPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
