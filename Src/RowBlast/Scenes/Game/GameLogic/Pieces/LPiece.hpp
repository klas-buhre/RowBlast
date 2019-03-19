#ifndef LPiece_hpp
#define LPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class LPiece: public Piece {
    public:
        LPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
