#ifndef TPiece_hpp
#define TPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class TPiece: public Piece {
    public:
        TPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
