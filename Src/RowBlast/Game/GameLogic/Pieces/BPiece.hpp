#ifndef BPiece_hpp
#define BPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class BPiece: public Piece {
    public:
        BPiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
