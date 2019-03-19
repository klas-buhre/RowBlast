#ifndef MirroredZPiece_hpp
#define MirroredZPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class MirroredZPiece: public Piece {
    public:
        MirroredZPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
