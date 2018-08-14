#ifndef SevenPiece_hpp
#define SevenPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class SevenPiece: public Piece {
    public:
        SevenPiece(Pht::IEngine& engine, const GameScene& scene);
        
        int GetNumEmptyTopRows() const override;
        bool NeedsUpAdjustmentInHud() const override;
    };
}

#endif
