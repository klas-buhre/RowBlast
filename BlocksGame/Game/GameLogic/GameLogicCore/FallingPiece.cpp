#include "FallingPiece.hpp"

using namespace BlocksGame;

void FallingPiece::Init(const Piece& pieceType, const Pht::Vec2& position, float speed) {
    mPosition = position;
    mRotation = Rotation::Deg0;
    mPieceType = &pieceType;
    ++mId;
    GoToFallingState(speed);
    mPreviousState = State::Falling;
    mPreviousIntY = static_cast<int>(mPosition.y);
}

void FallingPiece::ResetBetweenGames() {
    mId = 0;
    mPieceType = nullptr;
    mState = State::Falling;
    mPreviousState = State::Falling;
}

void FallingPiece::SetX(float x) {
    mPosition.x = x;
    mTimeSinceMovement = 0.0f;
}

void FallingPiece::SetY(float y) {
    mPreviousYStep = mPosition.y - y;
    mPreviousIntY = static_cast<int>(mPosition.y);
    mPosition.y = y;
    mTimeSinceMovement = 0.0f;
}

void FallingPiece::SetRotation(Rotation rotation) {
    mRotation = rotation;
    mTimeSinceMovement = 0.0f;
}

void FallingPiece::IncreaseTimeSinceMovement(float dt) {
    mTimeSinceMovement += dt;
}

void FallingPiece::IncreaseTimeSinceTouchdown(float dt) {
    mTimeSinceTouchdown += dt;
}

void FallingPiece::UpdateTime(float dt) {
    if (mState == State::BeingDraggedDown) {
        mTimeInDraggedDownState += dt;
    }
}

void FallingPiece::GoToFallingState(float speed) {
    mPreviousState = mState;
    mState = State::Falling;
    mSpeed = speed;
}

void FallingPiece::GoToBeingDraggedDownState() {
    mPreviousState = mState;
    mState = State::BeingDraggedDown;
    mTimeInDraggedDownState = 0.0f;
}

void FallingPiece::GoToLandingState(int yPosition) {
    mPreviousState = mState;
    mState = State::Landing;
    mSpeed = 0.0f;
    mTimeSinceTouchdown = 0.0f;
    mTimeSinceMovement = 0.0f;
    mPosition.y = yPosition;
}

Pht::IVec2 FallingPiece::GetIntPosition() const {
    return Pht::IVec2 {
        static_cast<int>(std::floor(mPosition.x)),
        static_cast<int>(std::floor(mPosition.y))
    };
}

Pht::Vec2 FallingPiece::GetRenderablePosition() const {
    return Pht::Vec2 {std::floor(mPosition.x), std::floor(mPosition.y)};
}

Pht::Vec2 FallingPiece::GetRenderablePositionSmoothY() const {
    return Pht::Vec2 {std::floor(mPosition.x), mPosition.y};
}
