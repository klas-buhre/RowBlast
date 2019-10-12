#include "DraggedPieceAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "CameraComponent.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "DraggedPiece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto goBackDuration = 0.15f;
    constexpr auto goUpDuration = 0.11f;

    Pht::StaticVector<Pht::Vec2, 20> offsetPoints {
        {0.0f, 0.0f},
        {0.1f, 0.005f},
        {0.2f, 0.01f},
        {0.3f, 0.02f},
        {0.35f, 0.035f},
        {0.4f, 0.05f},
        {0.45f, 0.065f},
        {0.5f, 0.08f},
        {0.55f, 0.115f},
        {0.6f, 0.15f},
        {0.65f, 0.225f},
        {0.7f, 0.3f},
        {0.75f, 0.41f},
        {0.8f, 0.52f},
        {0.85f, 0.62f},
        {0.9f, 0.7f},
        {0.95f, 0.87f},
        {1.0f, 1.0f},
    };
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

void DraggedPieceAnimation::StartGoUpAnimation() {
    mState = State::DraggedPieceGoingUp;
    mElapsedTime = 0.0f;
    mYOffset = 0.0f;
}

void DraggedPieceAnimation::StartGoBackAnimation(PreviewPieceIndex draggedPieceIndex) {
    mState = State::DraggedPieceGoingBack;
    mElapsedTime = 0.0f;
    
    auto& pieceType = mDraggedPiece.GetPieceType();
    auto cellSize = mScene.GetCellSize();
    mStartScale = 1.0f;
    mStopScale = GameHud::selectablePiecesScale * pieceType.GetPreviewCellSize() / cellSize;

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

    Pht::Vec2 pieceCenter {
        cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
        cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f
    };

    auto slotLocationVec2 =
        Pht::Vec2{slotLocation.x, slotLocation.y} + hudFrustumSize / 2.0f - pieceCenter * mStopScale;

    mStartPosition = mDraggedPiece.GetRenderablePosition();
    mStopPosition = screenLowerLeftWorldSpace + slotLocationVec2 * scaleFactor;
}

DraggedPieceAnimation::State DraggedPieceAnimation::Update() {
    switch (mState) {
        case State::Inactive:
            break;
        case State::DraggedPieceGoingUp:
            UpdateInDraggedPieceGoingUpState();
            break;
        case State::DraggedPieceGoingBack:
            UpdateInDraggedPieceGoingBackState();
            break;
    }
    
    return mState;
}

void DraggedPieceAnimation::UpdateInDraggedPieceGoingUpState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mElapsedTime > goUpDuration) {
        mState = State::Inactive;
        mYOffset = targetYOffsetInCells;
    } else {
        auto reversedNormalizedTime = 1.0f - (mElapsedTime / goUpDuration);
        mYOffset = targetYOffsetInCells * (1.0f - Pht::Lerp(reversedNormalizedTime, offsetPoints));
    }
}

void DraggedPieceAnimation::UpdateInDraggedPieceGoingBackState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    auto normalizedTime = mElapsedTime / goBackDuration;
    auto position = mStartPosition.Lerp(normalizedTime, mStopPosition);
    mDraggedPiece.SetPosition(position);
    
    auto scale = mStartScale * (1.0f - normalizedTime) + mStopScale * normalizedTime;
    mDraggedPiece.SetScale(scale);
    
    if (mElapsedTime > goBackDuration) {
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
