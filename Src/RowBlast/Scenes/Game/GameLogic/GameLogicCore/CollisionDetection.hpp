#ifndef CollisionDirection_hpp
#define CollisionDirection_hpp

// Game includes.
#include "Cell.hpp"
#include "Field.hpp"

namespace RowBlast {
    enum class Direction {
        Up,
        Down,
        Left,
        Right,
        None
    };
    
    enum class Intersection {
        Yes,
        NextWillBe,
        No
    };
    
    namespace CollisionDetection {
        Direction CalculateCollisionDirection(const Field::CollisionPoints& collisionPoints,
                                              const PieceBlocks& pieceBlocks,
                                              Rotation pieceRotation,
                                              const Pht::IVec2& position,
                                              const Field& field);
        Intersection SubCellsIntersect(const SubCell& field,
                                       const SubCell& piece,
                                       const Pht::IVec2& scanDirection,
                                       bool isScanStart);
        bool IsIllegalTiltedBondPosition(const Field& field,
                                         const Pht::IVec2& piecePosition,
                                         Rotation pieceRotation,
                                         const Piece& pieceType);
    }
}

#endif
