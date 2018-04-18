#ifndef FPiece_hpp
#define FPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class FPiece: public Piece {
    public:
        FPiece(Pht::IEngine& engine, const GameScene& scene);
        
        int GetNumEmptyTopRows() const override;
    };
}

#endif
