#ifndef ShortIPiece_hpp
#define ShortIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class ShortIPiece: public Piece {
    public:
        ShortIPiece(Pht::IEngine& engine, const GameScene& scene);
    };
}

#endif
