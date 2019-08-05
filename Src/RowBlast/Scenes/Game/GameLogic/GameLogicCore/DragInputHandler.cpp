#include "DragInputHandler.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "InputEvent.hpp"
#include "IInput.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "DraggedPiece.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto offsetYInCells = 1.5f;
}

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
    mDraggedPieceIndex = PreviewPieceIndex::None;
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
    if (TryBeginDrag(PreviewPieceIndex::Active, touchEvent)) {
        return;
    } else if (TryBeginDrag(PreviewPieceIndex::Selectable0, touchEvent)) {
        return;
    } else {
        TryBeginDrag(PreviewPieceIndex::Selectable1, touchEvent);
    }
}

bool DragInputHandler::TryBeginDrag(PreviewPieceIndex draggedPieceIndex,
                                    const Pht::TouchEvent& touchEvent) {
    if (GetPreviewPieceButton(draggedPieceIndex).OnTouch(touchEvent) == Pht::Button::Result::Down) {
        mState = State::Dragging;
        mDraggedPieceIndex = draggedPieceIndex;

        if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(draggedPieceIndex)) {
            previewPieceSceneObject->SetIsVisible(false);
        }

        auto* pieceType = GetPieceType(draggedPieceIndex);
        auto rotation = GetPieceRotation(draggedPieceIndex);
        mDraggedPiece.BeginDrag(*pieceType, rotation);
        UpdatePiecePosition(touchEvent);
                            
        if (!mGameLogic.BeginDraggingPiece(draggedPieceIndex)) {
            CancelDrag();
            return false;
        }
        return true;
    }
    
    return false;
}

void DragInputHandler::HandleOngoingTouch(const Pht::TouchEvent& touchEvent) {
    if (mState == State::Idle) {
        return;
    }
    
    UpdatePiecePosition(touchEvent);
}

void DragInputHandler::HandleTouchEnd(const Pht::TouchEvent& touchEvent) {
    if (mState == State::Idle) {
        return;
    }
    
    UpdatePiecePosition(touchEvent);
    
    // Reset the button state.
    GetPreviewPieceButton(mDraggedPieceIndex).Reset();

    if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(mDraggedPieceIndex)) {
        previewPieceSceneObject->SetIsVisible(true);
    }

    mGameLogic.StopDraggingPiece();
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mState = State::DragEnd;
}

void DragInputHandler::CancelDrag() {
    if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(mDraggedPieceIndex)) {
        previewPieceSceneObject->SetIsVisible(true);
    }

    GetPreviewPieceButton(mDraggedPieceIndex).Reset();
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mState = State::DragEnd;
}

void DragInputHandler::UpdatePiecePosition(const Pht::TouchEvent& touchEvent) {
    auto& cameraPosition = mScene.GetCamera().GetSceneObject().GetTransform().GetPosition();
    auto& renderer = mEngine.GetRenderer();
    auto& frustumSize = renderer.GetOrthographicFrustumSize();
    
    Pht::Vec2 screenLowerLeftWorldSpace {
        cameraPosition.x - frustumSize.x / 2.0f,
        cameraPosition.y - frustumSize.y / 2.0f
    };
    
    auto& screenInputSize = mEngine.GetInput().GetScreenInputSize();
    auto scaleFactor = frustumSize.y / screenInputSize.y;
    Pht::Vec2 touchLocation {touchEvent.mLocation.x, screenInputSize.y - touchEvent.mLocation.y};
    
    auto& pieceDimensions = mDraggedPiece.GetPieceType().GetDimensions(mDraggedPiece.GetRotation());
    auto pieceNumEmptyBottompRows = pieceDimensions.mYmin;
    auto offsetX = -static_cast<float>(pieceDimensions.mXmin + pieceDimensions.mXmax + 1) / 2.0f;
    auto offsetY = offsetYInCells - static_cast<float>(pieceNumEmptyBottompRows);
    auto cellSize = mScene.GetCellSize();
    Pht::Vec2 offset {offsetX * cellSize, offsetY * cellSize};
    auto position = screenLowerLeftWorldSpace + touchLocation * scaleFactor + offset;
    
    mDraggedPiece.SetPosition(position);
}

const Piece* DragInputHandler::GetPieceType(PreviewPieceIndex draggedPieceIndex) const {
    switch (draggedPieceIndex) {
        case PreviewPieceIndex::Active:
            return mGameLogic.GetPieceType();
        case PreviewPieceIndex::Selectable0:
            return mGameLogic.GetSelectablePieces()[0];
        case PreviewPieceIndex::Selectable1:
            return mGameLogic.GetSelectablePieces()[1];
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}

Rotation DragInputHandler::GetPieceRotation(PreviewPieceIndex draggedPieceIndex) const {
    auto& previewPieceHudRotations = mGameLogic.GetPreviewPieceHudRotations();
    
    switch (draggedPieceIndex) {
        case PreviewPieceIndex::Active:
            return previewPieceHudRotations.mActive;
        case PreviewPieceIndex::Selectable0:
            return previewPieceHudRotations.mSelectable0;
        case PreviewPieceIndex::Selectable1:
            return previewPieceHudRotations.mSelectable1;
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}

Pht::Button& DragInputHandler::GetPreviewPieceButton(PreviewPieceIndex draggedPieceIndex) const {
    auto& hud = mScene.GetHud();
    
    switch (draggedPieceIndex) {
        case PreviewPieceIndex::Active:
            return hud.GetActivePieceButton();
        case PreviewPieceIndex::Selectable0:
            return hud.GetSelectable0Button();
        case PreviewPieceIndex::Selectable1:
            return hud.GetSelectable1Button();
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}

Pht::SceneObject* DragInputHandler::GetPreviewPieceSceneObject(PreviewPieceIndex draggedPieceIndex) const {
    auto& hud = mScene.GetHud();
    
    switch (draggedPieceIndex) {
        case PreviewPieceIndex::Active:
            return hud.GetActivePreviewPieceSceneObject();
        case PreviewPieceIndex::Selectable0:
            return hud.GetSelectable0PreviewPieceSceneObject();
        case PreviewPieceIndex::Selectable1:
            return hud.GetSelectable1PreviewPieceSceneObject();
        case PreviewPieceIndex::None:
            assert(false);
            break;
    }
}
