#ifndef BigLPiece_hpp
#define BigLPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class BigLPiece: public Piece {
    public:
        BigLPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
