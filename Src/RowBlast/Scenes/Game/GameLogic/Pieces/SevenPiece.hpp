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
        
        int GetSpawnYPositionAdjustment() const override;
        bool NeedsRightAdjustmentInHud() const override;
        Rotation GetSpawnRotation() const override;
    };
}

#endif
