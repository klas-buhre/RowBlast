#ifndef PyramidPiece_hpp
#define PyramidPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class PyramidPiece: public Piece {
    public:
        PyramidPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
