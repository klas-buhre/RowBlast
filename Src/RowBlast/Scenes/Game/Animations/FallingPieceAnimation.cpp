#include "FallingPieceAnimation.hpp"

// Game includes.
#include "FallingPiece.hpp"
#include "MoveDefinitions.hpp"
#include "IGameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fastSpeed = 86.0f;
    constexpr auto slowSpeed = 67.0f;
    const Pht::Vec2 halfColumn {0.5f, 0.0f};
}

FallingPieceAnimation::FallingPieceAnimation(IGameLogic& gameLogic, FallingPiece& fallingPiece) :
    mGameLogic {gameLogic},
    mFallingPiece {fallingPiece} {}

void FallingPieceAnimation::Init() {
    mState = State::Inactive;
}

void FallingPieceAnimation::Start(const Movement& lastMovement) {
    mMovements.Clear();
    mMovementIndex = 0;
    
    for (auto* movement = &lastMovement; movement; movement = movement->GetPrevious()) {
        mMovements.PushBack(movement);
    }
    
    mMovements.Reverse();
    RemoveFirstMovementIfDetour();
    mState = State::Active;
    
    if (mFallingPiece.GetPieceType().IsBomb() || mFallingPiece.GetPieceType().IsRowBomb()) {
        mSpeed = slowSpeed;
    } else {
        mSpeed = fastSpeed;
    }
}

void FallingPieceAnimation::RemoveFirstMovementIfDetour() {
    if (mMovements.Size() > 1) {
        auto fallingPiecePos = mFallingPiece.GetRenderablePosition();
        auto* secondMovement = mMovements.At(1);
        if (fallingPiecePos.y == secondMovement->GetPosition().y) {
            auto* firstMovement = mMovements.Front();
            auto firstMovementDiff = fallingPiecePos - firstMovement->GetPosition();
            auto secondMovementDiff = fallingPiecePos - secondMovement->GetPosition();
            if (std::abs(secondMovementDiff.x) < std::abs(firstMovementDiff.x)) {
                mMovements.Erase(0);
            }
        }
    }
}

FallingPieceAnimation::State FallingPieceAnimation::Update(float dt) {
    switch (mState) {
        case State::Active:
            Animate(dt);
            return mState;
        case State::Landing:
            mState = State::Inactive;
            return State::Inactive;
        case State::Inactive:
            return State::Inactive;
    }
}

void FallingPieceAnimation::Animate(float dt) {
    if (mMovementIndex == mMovements.Size()) {
        LandFallingPiece();
        mState = State::Landing;
        return;
    }
    
    const auto* movement = mMovements.At(mMovementIndex);
    const auto& position = mFallingPiece.GetPosition();
    auto targetPosition = movement->GetPosition() + halfColumn;
    auto positionChange = mSpeed * dt;
    
    if (position.y > targetPosition.y) {
        Pht::Vec2 newPosition {position.x, position.y - positionChange};
        
        if (newPosition.y < targetPosition.y) {
            auto dtConsumed = (position.y - targetPosition.y) / mSpeed;
            mFallingPiece.SetY(targetPosition.y);
            NextMovement();
            Animate(dt - dtConsumed);
        } else {
            mFallingPiece.SetY(newPosition.y);
        }
    } else if (position.x > targetPosition.x) {
        Pht::Vec2 newPosition {position.x - positionChange, position.y};
        
        if (newPosition.x < targetPosition.x) {
            auto dtConsumed = (position.x - targetPosition.x) / mSpeed;
            mFallingPiece.SetX(targetPosition.x);
            NextMovement();
            Animate(dt - dtConsumed);
        } else {
            mFallingPiece.SetX(newPosition.x);
        }
    } else if (position.x < targetPosition.x) {
        Pht::Vec2 newPosition {position.x + positionChange, position.y};
        
        if (newPosition.x > targetPosition.x) {
            auto dtConsumed = (targetPosition.x - position.x) / mSpeed;
            mFallingPiece.SetX(targetPosition.x);
            NextMovement();
            Animate(dt - dtConsumed);
        } else {
            mFallingPiece.SetX(newPosition.x);
        }
    } else {
        NextMovement();
        Animate(dt);
    }
}

void FallingPieceAnimation::NextMovement() {
    const auto* previousMovement = mMovements.At(mMovementIndex);
    mFallingPiece.SetRotation(previousMovement->GetRotation());
    ++mMovementIndex;
}

void FallingPieceAnimation::LandFallingPiece() {
    const auto* finalMovement = mMovements.Back();
    const auto& landingPosition = finalMovement->GetPosition();
    
    mFallingPiece.SetRotation(finalMovement->GetRotation());
    mFallingPiece.SetX(landingPosition.x);
    mFallingPiece.SetY(landingPosition.y);
    
    auto secondToLastMovementIndex = mMovements.Size() - 2;

    auto finalMovementWasADrop =
        secondToLastMovementIndex >= 0 ?
        mMovements.At(secondToLastMovementIndex)->GetPosition().y > landingPosition.y : false;
    
    mGameLogic.OnFallingPieceAnimationFinished(finalMovementWasADrop);
}
