#ifndef ShortIPiece_hpp
#define ShortIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class ShortIPiece: public Piece {
    public:
        ShortIPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsDownAdjustmentInHud() const override;
    };
}

#endif
