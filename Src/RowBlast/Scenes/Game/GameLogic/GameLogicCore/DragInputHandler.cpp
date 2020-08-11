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
#include "DraggedPieceAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto dragBeginDistanceThreshold = 10.0f;
    constexpr auto dragBeginDistanceThresholdSquared =
        dragBeginDistanceThreshold * dragBeginDistanceThreshold;

    constexpr auto downTranslationDragBeginDistanceThreshold = 20.0f;
    constexpr auto downTranslationDragBeginDistanceThresholdSquared =
        downTranslationDragBeginDistanceThreshold * downTranslationDragBeginDistanceThreshold;
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
        case Pht::TouchState::Cancelled:
            HandleTouchCancelled();
            break;
        default:
            break;
    }
    
    renderer.SetHudMode(false);
    return mState;
}

void DragInputHandler::HandleTouchBegin(const Pht::TouchEvent& touchEvent) {
    switch (mState) {
        case State::Idle:
            HandleTouchBeginInIdleState(touchEvent);
            break;
        case State::TouchingPreviewPieceButton:
        case State::Dragging:
        case State::DragEnd:
            assert(false);
            break;
    }
}

void DragInputHandler::HandleOngoingTouch(const Pht::TouchEvent& touchEvent) {
    switch (mState) {
        case State::Idle:
            break;
        case State::TouchingPreviewPieceButton:
            HandleOngoingTouchInTouchingPreviewPieceButtonState(touchEvent);
            break;
        case State::Dragging:
            HandleOngoingTouchInDraggingState(touchEvent);
            break;
        case State::DragEnd:
            assert(false);
            break;
    }
}

void DragInputHandler::HandleTouchBeginInIdleState(const Pht::TouchEvent& touchEvent) {
    auto draggedPieceIndex = CalculateDraggedPieceIndexAtTouchBegin(touchEvent);
    if (draggedPieceIndex == PreviewPieceIndex::None) {
        return;
    }
    
    mState = State::TouchingPreviewPieceButton;
    mDraggedPieceIndex = draggedPieceIndex;
}

void
DragInputHandler::HandleOngoingTouchInTouchingPreviewPieceButtonState(const Pht::TouchEvent& touchEvent) {
    auto rotation = GetPieceRotation(mDraggedPieceIndex);
    if (IsTouchDistanceLongEnoughForDrag(touchEvent) &&
        mGameLogic.IsDragPieceAllowed(mDraggedPieceIndex, rotation)) {
        
        mState = State::Dragging;
        
        auto* pieceType = GetPieceType(mDraggedPieceIndex);
        if (pieceType == nullptr) {
            EndDrag();
            return;
        }

        auto rotation = GetPieceRotation(mDraggedPieceIndex);
        mDraggedPiece.BeginDrag(*pieceType, rotation);
        UpdatePiecePosition(touchEvent);
        mPreviousGridPosition = mDraggedPiece.GetFieldGridPosition();
        
        if (!mGameLogic.HandleBeginDraggingPiece(mDraggedPieceIndex)) {
            EndDrag();
            return;
        }
        
        mGameLogic.HandleDraggedPieceMoved();
    }
}

bool DragInputHandler::IsTouchDistanceLongEnoughForDrag(const Pht::TouchEvent& touchEvent) {
    auto dragDistanceSquared = touchEvent.mTranslation.x * touchEvent.mTranslation.x +
                               touchEvent.mTranslation.y * touchEvent.mTranslation.y;
    if (dragDistanceSquared > dragBeginDistanceThresholdSquared &&
        touchEvent.mTranslation.y < 0.0f) {
        
        return true;
    }
    
    if (dragDistanceSquared > downTranslationDragBeginDistanceThresholdSquared &&
        touchEvent.mTranslation.y > 0.0f) {
        
        return true;
    }

    return false;
}

void DragInputHandler::HandleOngoingTouchInDraggingState(const Pht::TouchEvent& touchEvent) {
    UpdatePiecePosition(touchEvent);
    NotifyGameLogicIfPieceMovedGridPosition();
}

void DragInputHandler::HandleTouchEnd(const Pht::TouchEvent& touchEvent) {
    switch (mState) {
        case State::Idle:
            break;
        case State::TouchingPreviewPieceButton:
            HandleTouchEndInTouchingPreviewPieceButtonState(touchEvent);
            break;
        case State::Dragging:
            HandleTouchEndInDraggingState(touchEvent);
            break;
        case State::DragEnd:
            assert(false);
            break;
    }
}

void DragInputHandler::HandleTouchEndInTouchingPreviewPieceButtonState(const Pht::TouchEvent& touchEvent) {
    mGameLogic.RotatePreviewPiece(mDraggedPieceIndex);
    EndDrag();
}
    
void DragInputHandler::HandleTouchEndInDraggingState(const Pht::TouchEvent& touchEvent) {
    UpdatePiecePosition(touchEvent);
    NotifyGameLogicIfPieceMovedGridPosition();
    mGameLogic.HandleDragPieceTouchEnd();
    EndDrag();
}

void DragInputHandler::EndDrag() {
    GetPreviewPieceButton(mDraggedPieceIndex).Reset();
    mDraggedPieceIndex = PreviewPieceIndex::None;
    mState = State::DragEnd;
}

void DragInputHandler::HandleTouchCancelled() {
    switch (mState) {
        case State::Idle:
        case State::DragEnd:
            break;
        case State::TouchingPreviewPieceButton:
            EndDrag();
            break;
        case State::Dragging:
            mGameLogic.EndPieceDrag();
            EndDrag();
            break;
    }
}

void DragInputHandler::NotifyGameLogicIfPieceMovedGridPosition() {
    auto gridPosition = mDraggedPiece.GetFieldGridPosition();
    if (gridPosition != mPreviousGridPosition) {
        mPreviousGridPosition = gridPosition;
        mGameLogic.HandleDraggedPieceMoved();
    }
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
    auto offsetY = DraggedPieceAnimation::targetYOffsetInCells - static_cast<float>(pieceNumEmptyBottompRows);
    auto cellSize = mScene.GetCellSize();
    Pht::Vec2 offset {offsetX * cellSize, offsetY * cellSize};
    auto position = screenLowerLeftWorldSpace + touchLocation * scaleFactor + offset;
    
    mDraggedPiece.SetPosition(position);
}

PreviewPieceIndex
DragInputHandler::CalculateDraggedPieceIndexAtTouchBegin(const Pht::TouchEvent& touchEvent) {
    if (GetPreviewPieceButton(PreviewPieceIndex::Active).OnTouch(touchEvent) == Pht::Button::Result::Down) {
        return PreviewPieceIndex::Active;
    }
    
    if (GetPreviewPieceButton(PreviewPieceIndex::Selectable0).OnTouch(touchEvent) == Pht::Button::Result::Down) {
        return PreviewPieceIndex::Selectable0;
    }

    if (GetPreviewPieceButton(PreviewPieceIndex::Selectable1).OnTouch(touchEvent) == Pht::Button::Result::Down) {
        return PreviewPieceIndex::Selectable1;
    }

    return PreviewPieceIndex::None;
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
    auto& previewPieceRotations = mGameLogic.GetPreviewPieceRotations();
    
    switch (draggedPieceIndex) {
        case PreviewPieceIndex::Active:
            return previewPieceRotations.mActive;
        case PreviewPieceIndex::Selectable0:
            return previewPieceRotations.mSelectable0;
        case PreviewPieceIndex::Selectable1:
            return previewPieceRotations.mSelectable1;
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
