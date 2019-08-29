#include "DraggedPieceAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "GameScene.hpp"
#include "DraggedPiece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 0.15f;
}

DraggedPieceAnimation::DraggedPieceAnimation(Pht::IEngine& engine,
                                             GameScene& scene,
                                             IGameLogic& gameLogic,
                                             DraggedPiece& draggedPiece) :
    mEngine {engine},
    mScene {scene},
    mGameLogic {gameLogic},
    mDraggedPiece {draggedPiece} {}

void DraggedPieceAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
}

void DraggedPieceAnimation::Start(PreviewPieceIndex draggedPieceIndex) {
    mState = State::Active;
    mElapsedTime = 0.0f;
    
    auto& cameraPosition = mScene.GetCamera().GetSceneObject().GetTransform().GetPosition();
    auto& renderer = mEngine.GetRenderer();
    auto& frustumSize = renderer.GetOrthographicFrustumSize();
    
    Pht::Vec2 screenLowerLeftWorldSpace {
        cameraPosition.x - frustumSize.x / 2.0f,
        cameraPosition.y - frustumSize.y / 2.0f
    };
    
    auto& hudFrustumSize = renderer.GetHudFrustumSize();
    auto scaleFactor = frustumSize.y / hudFrustumSize.y;
    auto slotLocation = GetPreviewPiecePosition(draggedPieceIndex);
    auto& pieceType = mDraggedPiece.GetPieceType();
    auto cellSize = mScene.GetCellSize();

    Pht::Vec2 pieceCenter {
        cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
        cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f
    };

    auto slotLocationVec2 =
        Pht::Vec2{slotLocation.x, slotLocation.y} + hudFrustumSize / 2.0f - pieceCenter;

    mStartPosition = mDraggedPiece.GetRenderablePosition();
    mStopPosition = screenLowerLeftWorldSpace + slotLocationVec2 * scaleFactor;
    
    mStartScale = 1.0f;
    mStopScale = pieceType.GetPreviewCellSize();
}

DraggedPieceAnimation::State DraggedPieceAnimation::Update() {
    switch (mState) {
        case State::Inactive:
            break;
        case State::Active:
            UpdateInActiveState();
            break;
    }
    
    return mState;
}

void DraggedPieceAnimation::UpdateInActiveState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    auto normalizedTime = mElapsedTime / animationDuration;
    auto position = mStartPosition.Lerp(normalizedTime, mStopPosition);
    mDraggedPiece.SetPosition(position);
    
    auto scale = mStartScale * (1.0f - normalizedTime) + mStopScale * normalizedTime;
    mDraggedPiece.SetScale(scale);
    
    if (mElapsedTime > animationDuration) {
        mGameLogic.OnDraggedPieceAnimationFinished();
        mState = State::Inactive;
    }
}

Pht::Vec3 DraggedPieceAnimation::GetPreviewPiecePosition(PreviewPieceIndex pieceIndex) const {
    auto& hud = mScene.GetHud();
    auto& selectablePiecesPositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();
    auto& selectablesContainerTransform = hud.GetSelectablePiecesContainer().GetTransform();
    auto& selectablesContainerPos = selectablesContainerTransform.GetPosition();
    auto& selectablesSceneObjectTransform = hud.GetSelectablePiecesSceneObject().GetTransform();
    auto selectablesSceneObjectScale = selectablesSceneObjectTransform.GetScale().x;

    Pht::Vec3 relativePosition;
    switch (pieceIndex) {
        case PreviewPieceIndex::Active:
            relativePosition = selectablePiecesPositionsInHud[1];
            break;
        case PreviewPieceIndex::Selectable0:
            relativePosition = selectablePiecesPositionsInHud[2];
            break;
        case PreviewPieceIndex::Selectable1:
            relativePosition = selectablePiecesPositionsInHud[3];
            break;
        case PreviewPieceIndex::None:
            relativePosition = {0.0f, 0.0f, 0.0f};
            break;
    }
    
    return selectablesContainerPos + relativePosition * selectablesSceneObjectScale;
}
