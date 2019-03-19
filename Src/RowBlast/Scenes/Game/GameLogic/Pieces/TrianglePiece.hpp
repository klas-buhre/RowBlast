#ifndef TrianglePiece_hpp
#define TrianglePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class TrianglePiece: public Piece {
    public:
        TrianglePiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
