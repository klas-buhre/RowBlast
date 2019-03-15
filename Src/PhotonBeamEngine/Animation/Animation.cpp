#include "Animation.hpp"

#include <assert.h>

#include "AnimationSystem.hpp"
#include "Fnv1Hash.hpp"
#include "SceneObject.hpp"

using namespace Pht;

const ComponentId Animation::id {Hash::Fnv1a("Animation")};

Animation::Animation(SceneObject& sceneObject,
                     const std::vector<Keyframe>& keyframes,
                     IAnimationSystem& animationSystem) :
    mSceneObject {sceneObject},
    mAnimationSystem {animationSystem} {
    
    SetKeyframes(keyframes);
}

Animation::~Animation() {
    mAnimationSystem.RemoveAnimation(*this);
}

void Animation::SetKeyframes(const std::vector<Keyframe>& keyframes) {
    mKeyframes.clear();

    for (auto& keyframe: keyframes) {
        if (mKeyframes.empty()) {
            assert(keyframe.mTime == 0.0f);
        } else {
            assert(keyframe.mTime > mKeyframes.back().mTime);
        }
        
        mKeyframes.push_back(keyframe);
    }
}

void Animation::Update(float dt) {
    if (!mIsPlaying) {
        return;
    }
    
    CalculateKeyframe(dt);
    
    if (mKeyframe != mPreviousKeyframe) {
        HandleKeyframeTransition();
    }
    
    if (mInterpolation == Interpolation::Linear) {
        UpdateLinearInterpolation();
    }
    
    PerformActionOnChildAnimations(Action::Update, mSceneObject, dt);
    
    mPreviousKeyframe = mKeyframe;
}

void Animation::CalculateKeyframe(float dt) {
    mElapsedTime += dt;
    
    for (auto i {1}; i < mKeyframes.size(); ++i) {
        auto& keyframe {mKeyframes[i - 1]};
        auto& nextKeyframe {mKeyframes[i]};
        if (mElapsedTime >= keyframe.mTime && mElapsedTime <= nextKeyframe.mTime) {
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
    auto& position {mKeyframe->mPosition};
    if (position.HasValue()) {
        transform.SetPosition(position.GetValue());
    }
    
    auto& scale {mKeyframe->mScale};
    if (scale.HasValue()) {
        transform.SetScale(scale.GetValue());
    }

    auto& rotation {mKeyframe->mRotation};
    if (rotation.HasValue()) {
        transform.SetRotation(rotation.GetValue());
    }

    auto isVisible {mKeyframe->mIsVisible};
    if (isVisible.HasValue()) {
        mSceneObject.SetIsVisible(isVisible.GetValue());
    }
    
    auto& callback {mKeyframe->mCallback};
    if (callback) {
        callback();
    }
}

void Animation::UpdateLinearInterpolation() {
    if (mKeyframe == nullptr || mNextKeyframe == nullptr) {
        return;
    }
    
    auto& transform {mSceneObject.GetTransform()};
    auto& position {mKeyframe->mPosition};
    auto& nextPosition {mNextKeyframe->mPosition};
    if (position.HasValue() && nextPosition.HasValue()) {
        transform.SetPosition(LerpVec3(position.GetValue(), nextPosition.GetValue()));
    }

    auto& scale {mKeyframe->mScale};
    auto& nextScale {mNextKeyframe->mScale};
    if (scale.HasValue() && nextScale.HasValue()) {
        transform.SetPosition(LerpVec3(scale.GetValue(), nextScale.GetValue()));
    }

    auto& rotation {mKeyframe->mRotation};
    auto& nextRotation {mNextKeyframe->mRotation};
    if (rotation.HasValue() && nextRotation.HasValue()) {
        transform.SetRotation(LerpVec3(rotation.GetValue(), nextRotation.GetValue()));
    }
}

Pht::Vec3 Animation::LerpVec3(const Pht::Vec3& keyframeValue, const Pht::Vec3& nextKeyframeValue) {
    auto timeBetweenKeyframes {mNextKeyframe->mTime - mKeyframe->mTime};
    auto elapsedInBetweenTime {std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f)};
    auto normalizedTime {elapsedInBetweenTime / timeBetweenKeyframes};
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * normalizedTime;
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
