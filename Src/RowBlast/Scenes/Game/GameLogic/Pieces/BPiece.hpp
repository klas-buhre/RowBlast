#ifndef BPiece_hpp
#define BPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class BPiece: public Piece {
    public:
        BPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
