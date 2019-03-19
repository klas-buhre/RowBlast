#ifndef FPiece_hpp
#define FPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class FPiece: public Piece {
    public:
        FPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
