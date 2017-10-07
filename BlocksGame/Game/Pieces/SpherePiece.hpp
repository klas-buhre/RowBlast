#ifndef SpherePiece_hpp
#define SpherePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class SpherePiece: public Piece {
    public:
        explicit SpherePiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool CanRotateAroundZ() const override;
    };
}

#endif

