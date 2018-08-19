#include "MoveDefinitions.hpp"

using namespace RowBlast;

Movement::Movement(const Pht::Vec2& position, Rotation rotation, const Movement* previous) :
    mPosition {position},
    mRotation {rotation},
    mPrevious {previous} {}

void MovingPiece::RotateClockwise() {
    auto numRotations {mPieceType.GetNumRotations()};
    mRotation = static_cast<Rotation>((static_cast<int>(mRotation) + 1) % numRotations);
}

void MovingPiece::RotateAntiClockwise() {
    auto newRotation {static_cast<int>(mRotation) - 1};
    
    if (newRotation < 0) {
        newRotation += mPieceType.GetNumRotations();
    }
    
    mRotation = static_cast<Rotation>(newRotation);
}

bool MovingPiece::IsAtTutorialMove(const Level::TutorialMove& tutorialMove) const {
    return mPosition == tutorialMove.mPosition && mRotation == tutorialMove.mRotation;
}

void ValidMoves::Clear() {
    mMoves.Clear();
    mMovements.Clear();
}
