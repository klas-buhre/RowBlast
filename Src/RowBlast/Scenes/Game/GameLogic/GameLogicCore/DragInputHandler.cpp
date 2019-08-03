#include "DragInputHandler.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "InputEvent.hpp"

// Game includes.
#include "DraggedPiece.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

DragInputHandler::DragInputHandler(Pht::IEngine& engine,
                                   IGameLogic& gameLogic,
                                   GameScene& gameScene,
                                   DraggedPiece& draggedPiece) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mScene {gameScene},
    mDraggedPiece {draggedPiece} {}

void DragInputHandler::Init() {
    mState = State::Idle;
    mDraggedPieceIndex = DraggedPieceIndex::None;
}

DragInputHandler::State DragInputHandler::HandleTouch(const Pht::TouchEvent& touchEvent) {
    if (mState == State::DragEnd) {
        mState = State::Idle;
    }

    auto& renderer = mEngine.GetRenderer();
    renderer.SetHudMode(true);
    
    switch (touchEvent.mState) {
        case Pht::TouchState::Begin:
            HandleTouchBegin(touchEvent);
            break;
        case Pht::TouchState::Ongoing:
            HandleOngoingTouch(touchEvent);
            break;
        case Pht::TouchState::End:
            HandleTouchEnd(touchEvent);
            break;
        default:
            break;
    }
    
    renderer.SetHudMode(false);
    return mState;
}

void DragInputHandler::HandleTouchBegin(const Pht::TouchEvent& touchEvent) {
    if (TryBeginDrag(DraggedPieceIndex::Active, touchEvent)) {
        return;
    } else if (TryBeginDrag(DraggedPieceIndex::Selectable0, touchEvent)) {
        return;
    } else {
        TryBeginDrag(DraggedPieceIndex::Selectable1, touchEvent);
    }
}

bool DragInputHandler::TryBeginDrag(DraggedPieceIndex draggedPieceIndex,
                                    const Pht::TouchEvent& touchEvent) {
    if (GetPreviewPieceButton(draggedPieceIndex).OnTouch(touchEvent) == Pht::Button::Result::Down) {
        mState = State::Dragging;
        mDraggedPieceIndex = draggedPieceIndex;
        if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(draggedPieceIndex)) {
            previewPieceSceneObject->SetIsVisible(false);
        }
        
        mGameLogic.ShowDraggedPiece();
        return true;
    }
    
    return false;
}

void DragInputHandler::HandleOngoingTouch(const Pht::TouchEvent& touchEvent) {
    if (mState == State::Idle) {
        return;
    }
}

void DragInputHandler::HandleTouchEnd(const Pht::TouchEvent& touchEvent) {
    if (mState == State::Idle) {
        return;
    }
    
    // Reset the button state.
    GetPreviewPieceButton(mDraggedPieceIndex).OnTouch(touchEvent);

    if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(mDraggedPieceIndex)) {
        previewPieceSceneObject->SetIsVisible(true);
    }
    
    mDraggedPieceIndex = DraggedPieceIndex::None;
    mState = State::DragEnd;
}

Pht::Button& DragInputHandler::GetPreviewPieceButton(DraggedPieceIndex draggedPieceIndex) const {
    auto& hud = mScene.GetHud();
    
    switch (draggedPieceIndex) {
        case DraggedPieceIndex::Active:
            return hud.GetActivePieceButton();
        case DraggedPieceIndex::Selectable0:
            return hud.GetSelectable0Button();
        case DraggedPieceIndex::Selectable1:
            return hud.GetSelectable1Button();
        case DraggedPieceIndex::None:
            assert(false);
            break;
    }
}

Pht::SceneObject* DragInputHandler::GetPreviewPieceSceneObject(DraggedPieceIndex draggedPieceIndex) const {
    auto& hud = mScene.GetHud();
    
    switch (draggedPieceIndex) {
        case DraggedPieceIndex::Active:
            return hud.GetActivePreviewPieceSceneObject();
        case DraggedPieceIndex::Selectable0:
            return hud.GetSelectable0PreviewPieceSceneObject();
        case DraggedPieceIndex::Selectable1:
            return hud.GetSelectable1PreviewPieceSceneObject();
        case DraggedPieceIndex::None:
            assert(false);
            break;
    }
}
