#ifndef SmallTrianglePiece_hpp
#define SmallTrianglePiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class SmallTrianglePiece: public Piece {
    public:
        SmallTrianglePiece(Pht::IEngine& engine, const GameScene& scene);
        
        bool PositionCanBeAdjusteInMovesSearch() const override;
    };
}

#endif
