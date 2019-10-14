#ifndef MiddleIPiece_hpp
#define MiddleIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class MiddleIPiece: public Piece {
    public:
        MiddleIPiece(Pht::IEngine& engine, const CommonResources& commonResources);
        
        Rotation GetSpawnRotation() const override;
    };
}

#endif
