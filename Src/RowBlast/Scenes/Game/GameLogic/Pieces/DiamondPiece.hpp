#ifndef DiamondPiece_hpp
#define DiamondPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class DiamondPiece: public Piece {
    public:
        DiamondPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
