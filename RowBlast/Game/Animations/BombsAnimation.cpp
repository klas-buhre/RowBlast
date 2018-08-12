#include "BombsAnimation.hpp"

// Engine includes.
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto bombAnimationDuration {4.5f};
    constexpr auto bombRotationAmplitude {22.0f};
    constexpr auto rowBombRotationSpeed {35.0f};
    constexpr auto emissiveAnimationDuration {1.5f};
    constexpr auto emissiveAmplitude {1.7f};
    
    template <typename T>
    void AnimateBombPreviewPieces(T& previewPieces, const Pht::Vec3& bombRotation) {
        for (auto& previewPiece: previewPieces) {
            if (previewPiece.mBombSceneObject) {
                previewPiece.mBombSceneObject->GetTransform().SetRotation(bombRotation);
            }
        }
    }
    
    template <typename T>
    void AnimateRowBombPreviewPieces(T& previewPieces, const Pht::Vec3& rowBombRotation) {
        for (auto& previewPiece: previewPieces) {
            if (previewPiece.mRowBombSceneObject) {
                previewPiece.mRowBombSceneObject->GetTransform().SetRotation(rowBombRotation);
            }
        }
    }
}

BombsAnimation::BombsAnimation(GameScene& scene,
                               PieceResources& pieceResources,
                               LevelResources& levelResources) :
    mScene {scene},
    mPieceResources {pieceResources},
    mLevelResources {levelResources} {}

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
    
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetBombRenderableObject(),
                                                   emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetTransparentBombRenderableObject(),
                                                   emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetRowBombRenderableObject(),
                                                   emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetTransparentRowBombRenderableObject(),
                                                   emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mLevelResources.GetLevelBombRenderable(),
                                                   emissive);
}

void BombsAnimation::AnimateBombRotation(float dt) {
    auto t {mAnimationTime * 2.0f * 3.1415f / bombAnimationDuration};
    auto xAngle {bombRotationAmplitude * sin(t) + 90.0f};
    auto yAngle {bombRotationAmplitude * cos(t)};
    
    mBombRotation = {xAngle, yAngle, 0.0f};
    
    AnimateBombPreviewPieces(mScene.GetHud().GetNextPreviewPieces(), mBombRotation);
    AnimateBombPreviewPieces(mScene.GetHud().GetSelectablePreviewPieces(), mBombRotation);
}

void BombsAnimation::AnimateRowBombRotation(float dt) {
    mRowBombRotation.y += rowBombRotationSpeed * dt;
    
    if (mRowBombRotation.y > 360.0f) {
        mRowBombRotation.y -= 360.0f;
    }
    
    AnimateRowBombPreviewPieces(mScene.GetHud().GetNextPreviewPieces(), mRowBombRotation);
    AnimateRowBombPreviewPieces(mScene.GetHud().GetSelectablePreviewPieces(), mRowBombRotation);
}
