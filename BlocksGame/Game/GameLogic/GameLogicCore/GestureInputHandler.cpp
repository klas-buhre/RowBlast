#include "GestureInputHandler.hpp"

// Engine includes.
#include "InputEvent.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameLogic.hpp"
#include "Level.hpp"

using namespace BlocksGame;

namespace {
    const float halfColumn {0.5f};
    const float inputUnitsPerColumn {26.5f * 1.25f}; // {26.5f * 10.0f / Field::numColumns};
    const float dropOnTouchEndMaxTime {1.2f};
}

GestureInputHandler::GestureInputHandler(IGameLogic& gameLogic, FallingPiece& fallingPiece) :
    mGameLogic {gameLogic},
    mFallingPiece {fallingPiece},
    mSwipeUpRecognizer {Pht::SwipeDirection::Up, inputUnitsPerColumn},
    mSwipeDownRecognizer {Pht::SwipeDirection::Down, inputUnitsPerColumn} {}

void GestureInputHandler::Init(const Level& level) {
    mLevel = &level;
}

void GestureInputHandler::HandleTouch(const Pht::TouchEvent& touchEvent) {
#ifdef DEBUG
    std::cout << "touch: state=" << static_cast<int>(touchEvent.mState)
              << " trans: " << touchEvent.mTranslation.x << ", " << touchEvent.mTranslation.y
              << " vel: " << touchEvent.mVelocity.x << ", " << touchEvent.mVelocity.y << std::endl;
#endif

    switch (touchEvent.mState) {
        case Pht::TouchState::Begin:
            HandleTouchBegin();
            return;
        case Pht::TouchState::End:
            if (ShouldDropFallingPieceOnTouchEnd()) {
                mGameLogic.DropFallingPiece();
                return;
            }
            ExitBeingDraggedDownState();
            if (TouchIsTap()) {
                mGameLogic.RotatePieceOrDetonateBomb(touchEvent);
                return;
            }
            if (TouchIsSwipeUp()) {
                mGameLogic.SwitchPiece();
                return;
            }
            break;
        case Pht::TouchState::Other:
            return;
        case Pht::TouchState::Ongoing:
            break;
    }

    if (mFallingPiece.GetId() != mPieceIdAtPanBegin) {
        return;
    }
    
    HandleDraggingDown(touchEvent);
    
    if (HandleSwipeDownOrUp(touchEvent) != Pht::IsSwipe::No) {
        return;
    }
    
    HandleDraggingSideways(touchEvent);
    
    if (mFallingPiece.IsBeingDraggedDown()) {
        MaybeExitBeingDraggedDownState();
    }

#ifdef DEBUG
    std::cout << "x=" << mFallingPiece.GetPosition().x << std::endl;
#endif
}

void GestureInputHandler::HandleTouchBegin() {
    mPreviousTranslationX = 0.0f;
    mPreviousTranslationY = 0.0f;
    mPieceIdAtPanBegin = mFallingPiece.GetId();
    mSwipeUpRecognizer.TouchBegin();
    mSwipeDownRecognizer.TouchBegin();
    mDiffState = DiffState::Accumulating;
    mAccumulatedDiff = 0.0f;
    mLastNonZeroDiffSign = 0;
    mTouchTriggeredDiffingState = false;
    mTouchContainsSwipeUp = false;
    mTouchTriggeredDragDownState = false;
    mDragDownContainedUpMovement = false;
}

Pht::IsSwipe GestureInputHandler::HandleSwipeDownOrUp(const Pht::TouchEvent& touchEvent) {
    auto isSwipeDown {mSwipeDownRecognizer.IsTouchContainingSwipe(touchEvent)};
    
    switch (isSwipeDown) {
        case Pht::IsSwipe::Yes:
            EnsureBeingDraggedDownState(touchEvent);
            return isSwipeDown;
        case Pht::IsSwipe::Maybe:
            return isSwipeDown;
        case Pht::IsSwipe::No:
            break;
    }
    
    auto isSwipeUp {mSwipeUpRecognizer.IsTouchContainingSwipe(touchEvent)};
    
    switch (isSwipeUp) {
        case Pht::IsSwipe::Yes:
            mTouchContainsSwipeUp = true;
            return isSwipeUp;
        case Pht::IsSwipe::Maybe:
            return isSwipeUp;
        case Pht::IsSwipe::No:
            break;
    }
    
    return Pht::IsSwipe::No;
}

bool GestureInputHandler::ShouldDropFallingPieceOnTouchEnd() {
    if (mFallingPiece.IsBeingDraggedDown() &&
        mFallingPiece.GetTimeInDraggedDownState() < dropOnTouchEndMaxTime &&
        !mDragDownContainedUpMovement) {
        return true;
    }
    
    if (mFallingPiece.IsLanding() &&
        mFallingPiece.GetPreviousState() == FallingPiece::State::BeingDraggedDown) {
        return true;
    }
    
    return false;
}

bool GestureInputHandler::TouchIsTap() {
    return !mTouchTriggeredDiffingState && !mTouchTriggeredDragDownState && !mTouchContainsSwipeUp;
}

bool GestureInputHandler::TouchIsSwipeUp() {
    return mTouchContainsSwipeUp && !mTouchTriggeredDiffingState && !mTouchTriggeredDragDownState;
}

void GestureInputHandler::HandleDraggingDown(const Pht::TouchEvent& touchEvent) {
    if (!mFallingPiece.IsBeingDraggedDown()) {
        return;
    }
    
    if (touchEvent.mTranslation.y - mPreviousTranslationY < 0.0f) {
        mDragDownContainedUpMovement = true;
        return;
    }
    
    float diff {
        (touchEvent.mTranslation.y - mPreviousTranslationY) / inputUnitsPerColumn -
        mFallingPiece.GetPreviousYStep()
    };
    
#ifdef DEBUG
    std::cout << "CalculateFallingPieceY: diff=" << diff << std::endl;
#endif

    if (diff < 0.0f) {
        return;
    }
    
    auto fallingPieceNewY {mFallingPiece.GetPosition().y - diff};
    auto ghostPieceRow {mGameLogic.GetGhostPieceRow()};
    
    if (fallingPieceNewY < ghostPieceRow) {
        mFallingPiece.GoToLandingState(ghostPieceRow);
    } else {
        mFallingPiece.SetY(fallingPieceNewY);
    }
    
    mPreviousTranslationY = touchEvent.mTranslation.y;
}

void GestureInputHandler::HandleDraggingSideways(const Pht::TouchEvent& touchEvent) {
    float diff {(touchEvent.mTranslation.x - mPreviousTranslationX) / inputUnitsPerColumn};
    auto diffSign {Pht::Sign(diff)};
    
    switch (mDiffState) {
        case DiffState::Accumulating:
            mAccumulatedDiff += diff;

#ifdef DEBUG
            std::cout << "mAccumulatedDiff=" << mAccumulatedDiff << std::endl;
#endif

            if (std::fabs(mAccumulatedDiff) > halfColumn) {
                float fallingPieceNewX {mFallingPiece.GetPosition().x + mAccumulatedDiff};
                
                if (mAccumulatedDiff > 0.0f) {
                    fallingPieceNewX -= halfColumn;
                } else {
                    fallingPieceNewX += halfColumn;
                }
                
                mGameLogic.SetFallingPieceXPosWithCollisionDetection(fallingPieceNewX);
                mDiffState = DiffState::Diffing;
                mTouchTriggeredDiffingState = true;
                mAccumulatedDiff = 0.0f;

#ifdef DEBUG
                std::cout << "Going to Diffing state" << std::endl;
#endif
            }
            break;
        case DiffState::Diffing:
            if (diffSign != 0 && diffSign != mLastNonZeroDiffSign) {
                std::cout << "Going to Accumulating state" << std::endl;
                
                mDiffState = DiffState::Accumulating;
                mAccumulatedDiff = diff;
                mFallingPiece.SetX(mFallingPiece.GetIntPosition().x + halfColumn);
            } else {
                auto fallingPieceNewX {mFallingPiece.GetPosition().x + diff};
                mGameLogic.SetFallingPieceXPosWithCollisionDetection(fallingPieceNewX);
            }
            break;
    }
    
    if (touchEvent.mState == Pht::TouchState::End) {
        mFallingPiece.SetX(mFallingPiece.GetIntPosition().x + halfColumn);
    }
    
    if (diffSign != 0) {
        mLastNonZeroDiffSign = diffSign;
    }
    
    mPreviousTranslationX = touchEvent.mTranslation.x;
}

void GestureInputHandler::EnsureBeingDraggedDownState(const Pht::TouchEvent& touchEvent) {
    if (!mFallingPiece.IsBeingDraggedDown() && touchEvent.mState != Pht::TouchState::End) {
        mFallingPiece.GoToBeingDraggedDownState();
        mTouchTriggeredDragDownState = true;
        mPreviousTranslationY = touchEvent.mTranslation.y;
    }
    
    mFallingPieceXAtDragDown = mFallingPiece.GetPosition().x;
    mDiffStateAtDragDown = mDiffState;
}

void GestureInputHandler::MaybeExitBeingDraggedDownState() {
    auto diff {std::fabs(mFallingPiece.GetPosition().x - mFallingPieceXAtDragDown)};
    
    if (mDiffStateAtDragDown == DiffState::Accumulating) {
        diff += halfColumn;
    }
    
    if (diff > halfColumn * 2) {
        ExitBeingDraggedDownState();
    }
}

void GestureInputHandler::ExitBeingDraggedDownState() {
    if (!mFallingPiece.IsBeingDraggedDown()) {
        return;
    }
    
#ifdef DEBUG
    std::cout << "Exit dragged down state." << std::endl;
#endif

    mFallingPiece.GoToFallingState(mLevel->GetSpeed());
    mDragDownContainedUpMovement = false;
}
