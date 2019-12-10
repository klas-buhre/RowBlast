#include "FallingPieceScaleAnimation.hpp"

// Engine includes.
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

namespace {
    constexpr auto scaleUpDuration = 0.27f;
    constexpr auto scaleDownDuration = 0.17f;
    
    Pht::StaticVector<Pht::Vec2, 20> scalePoints {
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

using namespace RowBlast;

FallingPieceScaleAnimation::FallingPieceScaleAnimation(GameScene& scene) :
    mScene {scene} {}

void FallingPieceScaleAnimation::Init() {
    mState = State::Inactive;
}

void FallingPieceScaleAnimation::StartScaleUp() {
    mState = State::ScalingUp;
    mElapsedTime = 0.0f;
}

void FallingPieceScaleAnimation::StartScaleDown() {
    mState = State::ScalingDown;
    mElapsedTime = 0.0f;
}

void FallingPieceScaleAnimation::Update(float dt) {
    switch (mState) {
        case State::ScalingUp:
            UpdateInScalingUpState(dt);
            break;
        case State::ScalingDown:
            UpdateInScalingDownState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void FallingPieceScaleAnimation::UpdateInScalingUpState(float dt) {
    auto& transform = mScene.GetPieceBlocks().GetContainerSceneObject().GetTransform();

    mElapsedTime += dt;
    if (mElapsedTime > scaleUpDuration) {
        mState = State::Inactive;
        transform.SetScale(1.0f);
    } else {
        auto reversedNormalizedTime = 1.0f - (mElapsedTime / scaleUpDuration);
        auto scale = 1.0f - Pht::Lerp(reversedNormalizedTime, scalePoints);
        transform.SetScale(scale);
    }
}

void FallingPieceScaleAnimation::UpdateInScalingDownState(float dt) {
    auto& transform = mScene.GetPieceBlocks().GetContainerSceneObject().GetTransform();

    mElapsedTime += dt;
    if (mElapsedTime > scaleDownDuration) {
        mState = State::Inactive;
        transform.SetScale(0.0f);
    } else {
        auto scale = 1.0f - (mElapsedTime / scaleDownDuration);
        transform.SetScale(scale);
    }
}
