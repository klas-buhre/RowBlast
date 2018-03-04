#include "FallingPieceAnimation.hpp"

// Game includes.
#include "FallingPiece.hpp"
#include "ValidMovesSearch.hpp"
#include "IGameLogic.hpp"

using namespace BlocksGame;

namespace {
    const auto speed {50.0f};
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
    
    for (auto* movement {&lastMovement}; movement; movement = movement->GetPrevious()) {
        mMovements.PushBack(movement);
    }
    
    mMovements.Reverse();
    mState = State::Active;
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
    
    const auto* nextMovement {mMovements.At(mMovementIndex)};
    const auto& position {mFallingPiece.GetPosition()};
    auto targetPosition {nextMovement->GetPosition() + halfColumn};
    auto positionChange {speed * dt};
    
    if (position.y > targetPosition.y) {
        Pht::Vec2 newPosition {position.x, position.y - positionChange};
        
        if (newPosition.y < targetPosition.y) {
            mFallingPiece.SetY(targetPosition.y);
            auto dtConsumed {(position.y - targetPosition.y) / speed};
            NextMovement();
            Animate(dt - dtConsumed);
        } else {
            mFallingPiece.SetY(newPosition.y);
        }
    } else if (position.x > targetPosition.x) {
        Pht::Vec2 newPosition {position.x - positionChange, position.y};
        
        if (newPosition.x < targetPosition.x) {
            mFallingPiece.SetX(targetPosition.x);
            auto dtConsumed {(position.x - targetPosition.x) / speed};
            NextMovement();
            Animate(dt - dtConsumed);
        } else {
            mFallingPiece.SetX(newPosition.x);
        }
    } else if (position.x < targetPosition.x) {
        Pht::Vec2 newPosition {position.x + positionChange, position.y};
        
        if (newPosition.x > targetPosition.x) {
            mFallingPiece.SetX(targetPosition.x);
            auto dtConsumed {(targetPosition.x - position.x) / speed};
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
    const auto* previousMovement {mMovements.At(mMovementIndex)};
    mFallingPiece.SetRotation(previousMovement->GetRotation());
    ++mMovementIndex;
}

void FallingPieceAnimation::LandFallingPiece() {
    const auto* finalMovement {mMovements.Back()};
    const auto& landingPosition {finalMovement->GetPosition()};
    
    mFallingPiece.SetRotation(finalMovement->GetRotation());
    mFallingPiece.SetX(landingPosition.x);
    mFallingPiece.SetY(landingPosition.y);
    
    auto secondToLastMovementIndex {mMovements.Size() - 2};
    assert(secondToLastMovementIndex >= 0);

    auto startParticleEffect {
        mMovements.At(secondToLastMovementIndex)->GetPosition().y > landingPosition.y
    };
    
    mGameLogic.OnFallingPieceAnimationFinished(startParticleEffect);
}
