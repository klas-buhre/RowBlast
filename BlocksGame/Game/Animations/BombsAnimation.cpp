#include "BombsAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "PieceResources.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto bombAnimationDuration {4.5f};
    constexpr auto bombRotationAmplitude {22.0f};
    constexpr auto rowBombRotationSpeed {35.0f};
    constexpr auto emissiveAnimationDuration {1.5f};
    constexpr auto emissiveAmplitude {1.7f};
    
    void SetEmissiveInRenderable(Pht::RenderableObject& renderableObject, float emissive) {
        Pht::Color emissiveColor {emissive, emissive, emissive};
        renderableObject.GetMaterial().SetEmissive(emissiveColor);
    }
}

BombsAnimation::BombsAnimation(GameScene& scene, PieceResources& pieceResources) :
    mScene {scene},
    mPieceResources {pieceResources} {}

void BombsAnimation::Init() {
    mBombRotation = {0.0f, 0.0f, 0.0f};
    mRowBombRotation = {0.0f, 0.0f, 0.0f};
    mAnimationTime = 0.0f;
    mEmissiveAnimationTime = 0.0f;
}

void BombsAnimation::Update(float dt) {
    mAnimationTime += dt;
    
    if (mAnimationTime > bombAnimationDuration) {
        mAnimationTime = 0.0f;
    }

    AnimateEmissive(dt);
    AnimateBombRotation(dt);
    AnimateRowBombRotation(dt);
}

void BombsAnimation::AnimateEmissive(float dt) {
    mEmissiveAnimationTime += dt;
    
    if (mEmissiveAnimationTime > emissiveAnimationDuration) {
        mEmissiveAnimationTime = 0.0f;
    }

    auto sineOfT {sin(mEmissiveAnimationTime * 2.0f * 3.1415f / emissiveAnimationDuration)};
    auto emissive {emissiveAmplitude * (sineOfT + 1.0f) / 2.0f};
    
    SetEmissiveInRenderable(mPieceResources.GetBombRenderableObject(), emissive);
    SetEmissiveInRenderable(mPieceResources.GetTransparentBombRenderableObject(), emissive);
    SetEmissiveInRenderable(mPieceResources.GetRowBombRenderableObject(), emissive);
    SetEmissiveInRenderable(mPieceResources.GetTransparentRowBombRenderableObject(), emissive);
}

void BombsAnimation::AnimateBombRotation(float dt) {
    auto t {mAnimationTime * 2.0f * 3.1415f / bombAnimationDuration};
    auto xAngle {bombRotationAmplitude * sin(t) + 90.0f};
    auto yAngle {bombRotationAmplitude * cos(t)};
    
    mBombRotation = {xAngle, yAngle, 0.0f};
    
    AnimateBombPreviewPieces(mScene.GetHud().GetNextPreviewPieces());
    AnimateBombPreviewPieces(mScene.GetHud().GetSelectablePreviewPieces());
}

void BombsAnimation::AnimateBombPreviewPieces(ThreePreviewPieces& previewPieces) {
    for (auto& previewPiece: previewPieces) {
        if (previewPiece.mBombSceneObject) {
            previewPiece.mBombSceneObject->GetTransform().SetRotation(mBombRotation);
        }
    }
}

void BombsAnimation::AnimateRowBombRotation(float dt) {
    mRowBombRotation.y += rowBombRotationSpeed * dt;
    
    if (mRowBombRotation.y > 360.0f) {
        mRowBombRotation.y -= 360.0f;
    }
    
    AnimateRowBombPreviewPieces(mScene.GetHud().GetNextPreviewPieces());
    AnimateRowBombPreviewPieces(mScene.GetHud().GetSelectablePreviewPieces());
}

void BombsAnimation::AnimateRowBombPreviewPieces(ThreePreviewPieces& previewPieces) {
    for (auto& previewPiece: previewPieces) {
        if (previewPiece.mRowBombSceneObject) {
            previewPiece.mRowBombSceneObject->GetTransform().SetRotation(mRowBombRotation);
        }
    }
}
