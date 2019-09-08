#include "PreviewPieceGroupAnimation.hpp"

// Engine includes.
#include "MathUtils.hpp"

using namespace RowBlast;

namespace {
    Pht::StaticVector<Pht::Vec2, 20> slidePoints {
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

void PreviewPieceGroupAnimation::Init() {
    mKind = Kind::None;
}

void PreviewPieceGroupAnimation::StartNextPieceAnimation(
    NextPreviewPieces& previewPieces,
    const NextPreviewPiecesPositionsConfig& piecePositionsConfig,
    float targetChangedScale) {

    mKind = Kind::NextPiece;
    mNextPreviewPieces = &previewPieces;
    mNextPiecePositionsConfig = piecePositionsConfig;
    mScaleChangeInitialScale = 1.0f;
    mScaleChangeFinalScale = targetChangedScale;
}

void PreviewPieceGroupAnimation::StartSwitchDuringNextPieceAnimation(
    SelectablePreviewPieces& previewPieces,
    const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig) {

    mKind = Kind::SwitchDuringNextPiece;
    mSelectablePreviewPieces = &previewPieces;
    mSelectablePreviewPiecesPositionsConfig = piecePositionsConfig;
}

void PreviewPieceGroupAnimation::StartSwitchPieceAnimation(
    SelectablePreviewPieces& previewPieces,
    const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig) {

    mKind = Kind::Switch;
    mSelectablePreviewPieces = &previewPieces;
    mSelectablePreviewPiecesPositionsConfig = piecePositionsConfig;
}

void PreviewPieceGroupAnimation::StartRemoveActivePieceAnimation(
    SelectablePreviewPieces& previewPieces,
    const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig) {

    mKind = Kind::RemoveActivePiece;
    mSelectablePreviewPieces = &previewPieces;
    mSelectablePreviewPiecesPositionsConfig = piecePositionsConfig;
}

void PreviewPieceGroupAnimation::Update(float normalizedElapsedTime) {
    if (mKind == Kind::None) {
        return;
    }

    auto reversedTime = 1.0f - normalizedElapsedTime;
    auto slideValue = Pht::Lerp(reversedTime, slidePoints);
    
    switch (mKind) {
        case Kind::NextPiece:
            UpdateNextPieceAnimation(slideValue);
            break;
        case Kind::SwitchDuringNextPiece:
            UpdateSwitchDuringNextPieceAnimation(slideValue);
            break;
        case Kind::Switch:
            UpdateSwitchPieceAnimation(slideValue);
            break;
        case Kind::RemoveActivePiece:
            UpdateRemoveActivePieceAnimation(slideValue);
            break;
        case Kind::None:
            break;
    }
}

void PreviewPieceGroupAnimation::UpdateNextPieceAnimation(float slideValue) {
    AnimatePiece(GetNextPreviewPiece(0),
                 mNextPiecePositionsConfig.mSlot0,
                 mNextPiecePositionsConfig.mRightSelectable,
                 slideValue,
                 Scaling::ScaleChange,
                 ZCurve::Yes);
    AnimatePiece(GetNextPreviewPiece(1),
                 mNextPiecePositionsConfig.mSlot1,
                 mNextPiecePositionsConfig.mSlot0,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(GetNextPreviewPiece(2),
                 mNextPiecePositionsConfig.mLower,
                 mNextPiecePositionsConfig.mSlot1,
                 slideValue,
                 Scaling::ScaleUp);
}

void PreviewPieceGroupAnimation::UpdateSwitchDuringNextPieceAnimation(float slideValue) {
    AnimatePiece(GetSelectablePreviewPiece(0),
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 mSelectablePreviewPiecesPositionsConfig.mLeft,
                 slideValue,
                 Scaling::ScaleDown);
    AnimatePiece(GetSelectablePreviewPiece(1),
                 mSelectablePreviewPiecesPositionsConfig.mSlot1,
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(GetSelectablePreviewPiece(2),
                 mSelectablePreviewPiecesPositionsConfig.mSlot2,
                 mSelectablePreviewPiecesPositionsConfig.mSlot1,
                 slideValue,
                 Scaling::NoScaling);
}

void PreviewPieceGroupAnimation::UpdateSwitchPieceAnimation(float slideValue) {
    AnimatePiece(GetSelectablePreviewPiece(0),
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 mSelectablePreviewPiecesPositionsConfig.mLeft,
                 slideValue,
                 Scaling::ScaleDown);
    AnimatePiece(GetSelectablePreviewPiece(1),
                 mSelectablePreviewPiecesPositionsConfig.mSlot1,
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(GetSelectablePreviewPiece(2),
                 mSelectablePreviewPiecesPositionsConfig.mSlot2,
                 mSelectablePreviewPiecesPositionsConfig.mSlot1,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(GetSelectablePreviewPiece(3),
                 mSelectablePreviewPiecesPositionsConfig.mRight,
                 mSelectablePreviewPiecesPositionsConfig.mSlot2,
                 slideValue,
                 Scaling::ScaleUp);
}

void PreviewPieceGroupAnimation::UpdateRemoveActivePieceAnimation(float slideValue) {
    AnimatePiece(GetSelectablePreviewPiece(0),
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 mSelectablePreviewPiecesPositionsConfig.mSlot0,
                 slideValue,
                 Scaling::ScaleDown);
}

void PreviewPieceGroupAnimation::AnimatePiece(PreviewPiece& previewPiece,
                                              const Pht::Vec3& startPosition,
                                              const Pht::Vec3& stopPosition,
                                              float slideFunctionValue,
                                              Scaling scaling,
                                              ZCurve zCurve) {
    auto diff = stopPosition - startPosition;
    auto& transform = previewPiece.mSceneObject->GetTransform();
    auto position = stopPosition - diff * slideFunctionValue;
    if (zCurve == ZCurve::Yes) {
        auto zCurveAdd = 1.5f * std::sin(slideFunctionValue * 3.1415f);
        position.z += zCurveAdd;
    }
    
    transform.SetPosition(position);
    
    switch (scaling) {
        case Scaling::ScaleUp: {
            auto finalScale = previewPiece.mScale;
            auto scale = (1.0f - slideFunctionValue) * finalScale;
            transform.SetScale(scale);
            break;
        }
        case Scaling::ScaleDown: {
            auto initialScale = previewPiece.mScale;
            auto scale = slideFunctionValue * initialScale;
            transform.SetScale(scale);
            break;
        }
        case Scaling::ScaleChange: {
            auto initialScale = previewPiece.mScale * mScaleChangeInitialScale;
            auto finalScale = previewPiece.mScale * mScaleChangeFinalScale;
            auto scale = slideFunctionValue * initialScale + (1.0f - slideFunctionValue) * finalScale;
            transform.SetScale(scale);
            break;
        }
        case Scaling::NoScaling:
            break;
    }
}

PreviewPiece& PreviewPieceGroupAnimation::GetNextPreviewPiece(int previewPieceIndex) {
    assert(mNextPreviewPieces);
    assert(previewPieceIndex < mNextPreviewPieces->size());
    return (*mNextPreviewPieces)[previewPieceIndex];
}

PreviewPiece& PreviewPieceGroupAnimation::GetSelectablePreviewPiece(int previewPieceIndex) {
    assert(mSelectablePreviewPieces);
    assert(previewPieceIndex < mSelectablePreviewPieces->size());
    return (*mSelectablePreviewPieces)[previewPieceIndex];
}
