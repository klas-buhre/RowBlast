#ifndef MirroredSevenPiece_hpp
#define MirroredSevenPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class MirroredSevenPiece: public Piece {
    public:
        MirroredSevenPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool NeedsLeftAdjustmentInHud() const override;
        Rotation GetSpawnRotation() const override;
    };
}

#endif
