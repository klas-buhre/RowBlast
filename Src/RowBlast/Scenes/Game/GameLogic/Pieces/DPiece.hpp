#ifndef DPiece_hpp
#define DPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class DPiece: public Piece {
    public:
        DPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
