#ifndef SmallTrianglePiece_hpp
#define SmallTrianglePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class SmallTrianglePiece: public Piece {
    public:
        SmallTrianglePiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        bool PositionCanBeAdjusteInMovesSearch() const override;
    };
}

#endif
