#include "AsteroidAnimation.hpp"

// Engine includes.
#include "SceneObject.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {3.8f};
    constexpr auto rotationAmplitude {19.5f};
}

void AsteroidAnimation::Init() {
    mAsteroidSceneObject = nullptr;
    mAnimationTime = 0.0f;
}

void AsteroidAnimation::Update(float dt) {
    if (mAsteroidSceneObject == nullptr) {
        return;
    }
    
    mAnimationTime += dt;
    
    if (mAnimationTime > animationDuration) {
        mAnimationTime = 0.0f;
    }
    
    auto t {mAnimationTime * 2.0f * 3.1415f / animationDuration};
    auto yAngle {rotationAmplitude * std::cos(t) + 45.0f};
    
    mAsteroidSceneObject->GetTransform().SetRotation({-25.0f, yAngle, -12.0f});
}
