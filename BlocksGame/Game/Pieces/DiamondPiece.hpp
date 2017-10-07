#ifndef DiamondPiece_hpp
#define DiamondPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class DiamondPiece: public Piece {
    public:
        DiamondPiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool CanRotateAroundZ() const override;
    };
}

#endif
