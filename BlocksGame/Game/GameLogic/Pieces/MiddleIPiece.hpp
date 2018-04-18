#ifndef MiddleIPiece_hpp
#define MiddleIPiece_hpp

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class MiddleIPiece: public Piece {
    public:
        MiddleIPiece(Pht::IEngine& engine, const GameScene& scene);
        
        int GetNumEmptyTopRows() const override;
    };
}

#endif
