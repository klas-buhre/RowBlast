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

void PreviewPieceGroupAnimation::Start(Kind kind,
                                       ThreePreviewPieces& previewPieces,
                                       const PreviewPiecePositionsConfig& piecePositionsConfig) {
    assert(kind != Kind::None);
    
    mKind = kind;
    mPreviewPieces = &previewPieces;
    mPiecePositionsConfig = piecePositionsConfig;
}

void PreviewPieceGroupAnimation::Update(float normalizedElapsedTime) {
    auto reversedTime {1.0f - normalizedElapsedTime};
    auto slideValue {Pht::Lerp(reversedTime, slidePoints)};
    
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
        case Kind::None:
            assert(!"Invalid animation kind.");
            break;
    }
}

void PreviewPieceGroupAnimation::UpdateNextPieceAnimation(float slideValue) {
    AnimatePiece(0,
                 mPiecePositionsConfig.mLeft.x,
                 mPiecePositionsConfig.mSlot1.x,
                 slideValue,
                 Scaling::ScaleUp);
    AnimatePiece(1,
                 mPiecePositionsConfig.mSlot1.x,
                 mPiecePositionsConfig.mSlot2.x,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(2,
                 mPiecePositionsConfig.mSlot2.x,
                 mPiecePositionsConfig.mRight.x,
                 slideValue,
                 Scaling::NoScaling);
}

void PreviewPieceGroupAnimation::UpdateSwitchDuringNextPieceAnimation(float slideValue) {
    AnimatePiece(1,
                 mPiecePositionsConfig.mSlot1.x,
                 mPiecePositionsConfig.mSlot2.x,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(2,
                 mPiecePositionsConfig.mSlot2.x,
                 mPiecePositionsConfig.mRight.x,
                 slideValue,
                 Scaling::ScaleDown);
}

void PreviewPieceGroupAnimation::UpdateSwitchPieceAnimation(float slideValue) {
    AnimatePiece(0,
                 mPiecePositionsConfig.mLeft.x,
                 mPiecePositionsConfig.mSlot1.x,
                 slideValue,
                 Scaling::ScaleUp);
    AnimatePiece(1,
                 mPiecePositionsConfig.mSlot1.x,
                 mPiecePositionsConfig.mSlot2.x,
                 slideValue,
                 Scaling::NoScaling);
    AnimatePiece(2,
                 mPiecePositionsConfig.mSlot2.x,
                 mPiecePositionsConfig.mRight.x,
                 slideValue,
                 Scaling::ScaleDown);
}

void PreviewPieceGroupAnimation::AnimatePiece(int previewPieceIndex,
                                              float xStart,
                                              float xStop,
                                              float slideFunctionValue,
                                              Scaling scaling) {
    auto distance {xStop - xStart};
    auto& transform {GetSceneObject(previewPieceIndex).GetTransform()};
    auto position {transform.GetPosition()};
    
    position.x = xStop - distance * slideFunctionValue;
    transform.SetPosition(position);
    
    switch (scaling) {
        case Scaling::ScaleUp: {
            auto finalScale {GetPreviewPiece(previewPieceIndex).mScale};
            auto scale {(1.0f - slideFunctionValue) * finalScale};
            transform.SetScale(scale);
            break;
        }
        case Scaling::ScaleDown: {
            auto initialScale {GetPreviewPiece(previewPieceIndex).mScale};
            auto scale {slideFunctionValue * initialScale};
            transform.SetScale(scale);
            break;
        }
        case Scaling::NoScaling:
            break;
    }
}

PreviewPiece& PreviewPieceGroupAnimation::GetPreviewPiece(int previewPieceIndex) {
    assert(mPreviewPieces);
    assert(previewPieceIndex < mPreviewPieces->size());
    return (*mPreviewPieces)[previewPieceIndex];
}

Pht::SceneObject& PreviewPieceGroupAnimation::GetSceneObject(int previewPieceIndex) {
    return GetPreviewPiece(previewPieceIndex).mSceneObjects->GetContainerSceneObject();
}
