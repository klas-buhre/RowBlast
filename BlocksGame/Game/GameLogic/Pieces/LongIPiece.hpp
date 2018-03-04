#ifndef LongIPiece_hpp
#define LongIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class LongIPiece: public Piece {
    public:
        LongIPiece(Pht::IEngine& engine, const GameScene& scene);
        
        int GetNumEmptyTopRows() const override;
    };
}

#endif
