#ifndef LongIPiece_hpp
#define LongIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class LongIPiece: public Piece {
    public:
        LongIPiece(Pht::IEngine& engine, const CommonResources& commonResources);
    };
}

#endif
