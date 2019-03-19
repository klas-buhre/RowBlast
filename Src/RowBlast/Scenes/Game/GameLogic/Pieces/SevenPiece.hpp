#ifndef SevenPiece_hpp
#define SevenPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class SevenPiece: public Piece {
    public:
        SevenPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsUpAdjustmentInHud() const override;
    };
}

#endif
