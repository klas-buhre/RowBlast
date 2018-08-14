#ifndef DPiece_hpp
#define DPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class DPiece: public Piece {
    public:
        DPiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
