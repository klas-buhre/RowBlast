#include "DragInputHandler.hpp"

// Game includes.
#include "DraggedPiece.hpp"

using namespace RowBlast;

DragInputHandler::DragInputHandler(IGameLogic& gameLogic,
                                   GameScene& gameScene,
                                   DraggedPiece& draggedPiece) :
    mGameLogic {gameLogic},
    mScene {gameScene},
    mDraggedPiece {draggedPiece} {}

void DragInputHandler::Init() {
    mState = State::Idle;
}

DragInputHandler::State DragInputHandler::HandleTouch(const Pht::TouchEvent& touchEvent) {
    return State::Idle;
}
