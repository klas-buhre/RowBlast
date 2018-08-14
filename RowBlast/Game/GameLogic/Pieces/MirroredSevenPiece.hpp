#ifndef MirroredSevenPiece_hpp
#define MirroredSevenPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class MirroredSevenPiece: public Piece {
    public:
        MirroredSevenPiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
