#include "DraggedPiece.hpp"

// Game includes.
#include "Piece.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

DraggedPiece::DraggedPiece(const GameScene& scene) :
    mScene {scene} {}

void DraggedPiece::BeginDrag(const Piece& pieceType, Rotation rotation) {
    mPieceType = &pieceType;
    mRotation = rotation;
    mScale = 1.0f;
}

void DraggedPiece::SetPosition(const Pht::Vec2& position) {
    mPosition = position;
}

Pht::IVec2 DraggedPiece::GetFieldGridPosition() const {
    auto gridPosition = (mPosition - mScene.GetFieldLoweLeft()) / mScene.GetCellSize();
    
    return Pht::IVec2 {
        static_cast<int>(std::floor(gridPosition.x + 0.5f)),
        static_cast<int>(std::floor(gridPosition.y + 0.5f))
    };
}
