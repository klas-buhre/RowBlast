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
        
        auto* pieceType = GetPieceType(draggedPieceIndex);
        auto rotation = GetPieceRotation(draggedPieceIndex);
        mDraggedPiece.BeginDrag(*pieceType, rotation);
        mDraggedPiece.SetPosition(CalculatePiecePosition(touchEvent));
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
    
    mDraggedPiece.SetPosition(CalculatePiecePosition(touchEvent));
}

void DragInputHandler::HandleTouchEnd(const Pht::TouchEvent& touchEvent) {
    if (mState == State::Idle) {
        return;
    }
    
    mDraggedPiece.SetPosition(CalculatePiecePosition(touchEvent));
    
    // Reset the button state.
    GetPreviewPieceButton(mDraggedPieceIndex).Reset();

    if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(mDraggedPieceIndex)) {
        previewPieceSceneObject->SetIsVisible(true);
    }
    
    mGameLogic.StopDraggingPiece();
    mDraggedPieceIndex = DraggedPieceIndex::None;
    mState = State::DragEnd;
}

void DragInputHandler::CancelDrag() {
    if (auto* previewPieceSceneObject = GetPreviewPieceSceneObject(mDraggedPieceIndex)) {
        previewPieceSceneObject->SetIsVisible(true);
    }

    GetPreviewPieceButton(mDraggedPieceIndex).Reset();
    mDraggedPieceIndex = DraggedPieceIndex::None;
    mState = State::DragEnd;
}

Pht::Vec2 DragInputHandler::CalculatePiecePosition(const Pht::TouchEvent& touchEvent) const {
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
    return screenLowerLeftWorldSpace + touchLocation * scaleFactor + offset;
}

const Piece* DragInputHandler::GetPieceType(DraggedPieceIndex draggedPieceIndex) const {
    switch (draggedPieceIndex) {
        case DraggedPieceIndex::Active:
            return mGameLogic.GetPieceType();
        case DraggedPieceIndex::Selectable0:
            return mGameLogic.GetSelectablePieces()[0];
        case DraggedPieceIndex::Selectable1:
            return mGameLogic.GetSelectablePieces()[1];
        case DraggedPieceIndex::None:
            assert(false);
            break;
    }
}

Rotation DragInputHandler::GetPieceRotation(DraggedPieceIndex draggedPieceIndex) const {
    auto& previewPieceHudRotations = mGameLogic.GetPreviewPieceHudRotations();
    
    switch (draggedPieceIndex) {
        case DraggedPieceIndex::Active:
            return previewPieceHudRotations.mActive;
        case DraggedPieceIndex::Selectable0:
            return previewPieceHudRotations.mSelectable0;
        case DraggedPieceIndex::Selectable1:
            return previewPieceHudRotations.mSelectable1;
        case DraggedPieceIndex::None:
            assert(false);
            break;
    }
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
