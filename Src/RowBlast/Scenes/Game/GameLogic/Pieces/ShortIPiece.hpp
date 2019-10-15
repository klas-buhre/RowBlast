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
        
        bool NeedsLeftAdjustmentInHud() const override;
        Rotation GetSpawnRotation() const override;
    };
}

#endif
