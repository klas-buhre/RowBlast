#include "Animation.hpp"

#include <assert.h>

#include "AnimationSystem.hpp"
#include "Fnv1Hash.hpp"
#include "SceneObject.hpp"

using namespace Pht;

const ComponentId Animation::id {Hash::Fnv1a("Animation")};

Animation::Animation(SceneObject& sceneObject, IAnimationSystem& animationSystem) :
    mSceneObject {sceneObject},
    mAnimationSystem {animationSystem} {}

Animation::~Animation() {
    mAnimationSystem.RemoveAnimation(*this);
}

void Animation::AddKeyframe(const Keyframe& keyframe) {
    mKeyframes.push_back(keyframe);
}

void Animation::Update(float dt) {
    if (!mIsPlaying) {
        return;
    }
    
    CalculateKeyframe(dt);
    
    if (mKeyframe != mPreviousKeyframe) {
        HandleKeyframeTransition();
    }
    
    mPreviousKeyframe = mKeyframe;
    
    PerformActionOnChildAnimations(Action::Update, mSceneObject, dt);
}

void Animation::CalculateKeyframe(float dt) {
    mElapsedTime += dt;
    
    for (auto i {1}; i < mKeyframes.size(); ++i) {
        auto& keyframe {mKeyframes[i - 1]};
        auto& nextKeyframe {mKeyframes[i]};
        if (mElapsedTime >= keyframe.GetTime() && mElapsedTime <= nextKeyframe.GetTime()) {
            mKeyframe = &keyframe;
            mNextKeyframe = &nextKeyframe;
            return;
        }
    }
    
    mElapsedTime = 0.0f;
    mKeyframe = &mKeyframes[0];
    mNextKeyframe = &mKeyframes[1];
}

void Animation::HandleKeyframeTransition() {
    if (mKeyframe == nullptr) {
        return;
    }
    
    auto& transform {mSceneObject.GetTransform()};
    auto& position {mKeyframe->GetPosition()};
    if (position.HasValue()) {
        transform.SetPosition(position.GetValue());
    }
    
    auto& scale {mKeyframe->GetScale()};
    if (scale.HasValue()) {
        transform.SetScale(scale.GetValue());
    }

    auto& rotation {mKeyframe->GetRotation()};
    if (rotation.HasValue()) {
        transform.SetRotation(rotation.GetValue());
    }

    auto isVisible {mKeyframe->IsVisible()};
    if (isVisible.HasValue()) {
        mSceneObject.SetIsVisible(isVisible.GetValue());
    }
}

void Animation::Play() {
    assert(mKeyframes.size() >= 2);

    mIsPlaying = true;
    PerformActionOnChildAnimations(Action::Play, mSceneObject);
}

void Animation::Pause() {
    mIsPlaying = false;
    PerformActionOnChildAnimations(Action::Pause, mSceneObject);
}

void Animation::Stop() {
    mElapsedTime = 0.0f;
    mIsPlaying = false;
    mPreviousKeyframe = nullptr;
    mKeyframe = nullptr;
    mNextKeyframe = nullptr;
    
    PerformActionOnChildAnimations(Action::Stop, mSceneObject);
}

void Animation::PerformActionOnChildAnimations(Action action, SceneObject& sceneObject, float dt) {
    for (auto* childSceneObject: sceneObject.GetChildren()) {
        auto* childAnimation {childSceneObject->GetComponent<Animation>()};
        if (childAnimation) {
            switch (action) {
                case Action::Update:
                    childAnimation->Update(dt);
                    break;
                case Action::Play:
                    childAnimation->Play();
                    break;
                case Action::Pause:
                    childAnimation->Pause();
                    break;
                case Action::Stop:
                    childAnimation->Stop();
                    break;
            }
        } else {
            PerformActionOnChildAnimations(action, *childSceneObject, dt);
        }
    }
}
