#include "DraggedPiece.hpp"

// Game includes.
#include "Piece.hpp"

using namespace RowBlast;

void DraggedPiece::BeginDrag(const Piece& pieceType, Rotation rotation) {
    mPieceType = &pieceType;
    mRotation = rotation;
}

void DraggedPiece::SetPosition(const Pht::Vec2& position) {
    mPosition = position;
}

Pht::IVec2 DraggedPiece::GetFieldGridPosition() const {
    return Pht::IVec2 {
        static_cast<int>(std::floor(mPosition.x)),
        static_cast<int>(std::floor(mPosition.y))
    };
}
