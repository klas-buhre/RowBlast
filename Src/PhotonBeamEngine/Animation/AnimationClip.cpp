#include "AnimationClip.hpp"

#include <assert.h>

#include "SceneObject.hpp"

using namespace Pht;

AnimationClip::AnimationClip(const std::vector<Keyframe>& keyframes) {
    for (auto& keyframe: keyframes) {
        if (mKeyframes.empty()) {
            assert(keyframe.mTime == 0.0f);
        } else {
            assert(keyframe.mTime > mKeyframes.back().mTime);
        }
        
        mKeyframes.push_back(keyframe);
    }
}

void AnimationClip::Update(float dt) {
    if (!mIsPlaying) {
        return;
    }
    
    if (!CalculateKeyframe(dt)) {
        return;
    }

    if (mKeyframe != mPreviousKeyframe) {
        HandleKeyframeTransition(*mKeyframe);
    }
    
    if (mInterpolation != Interpolation::None) {
        UpdateInterpolation();
    }
    
    mPreviousKeyframe = mKeyframe;
}

bool AnimationClip::CalculateKeyframe(float dt) {
    mElapsedTime += dt;
    
    for (auto i = 1; i < mKeyframes.size(); ++i) {
        auto& keyframe = mKeyframes[i - 1];
        auto& nextKeyframe = mKeyframes[i];
        if (mElapsedTime >= keyframe.mTime && mElapsedTime <= nextKeyframe.mTime) {
            mKeyframe = &keyframe;
            mNextKeyframe = &nextKeyframe;
            return true;
        }
    }
    
    HandleKeyframeTransition(mKeyframes.back());
    
    switch (mWrapMode) {
        case WrapMode::Once:
            Stop();
            return false;
        case WrapMode::Loop:
            mElapsedTime = 0.0f;
            mKeyframe = &mKeyframes[0];
            mNextKeyframe = &mKeyframes[1];
            return true;
    }
}

void AnimationClip::HandleKeyframeTransition(const Keyframe& newKeyframe) {
    auto& transform = mSceneObject->GetTransform();
    auto& position = newKeyframe.mPosition;
    if (position.HasValue()) {
        transform.SetPosition(position.GetValue());
    }
    
    auto& scale = newKeyframe.mScale;
    if (scale.HasValue()) {
        transform.SetScale(scale.GetValue());
    }

    auto& rotation = newKeyframe.mRotation;
    if (rotation.HasValue()) {
        transform.SetRotation(rotation.GetValue());
    }

    auto isVisible = newKeyframe.mIsVisible;
    if (isVisible.HasValue()) {
        mSceneObject->SetIsVisible(isVisible.GetValue());
    }
    
    auto& callback = newKeyframe.mCallback;
    if (callback) {
        callback();
    }
}

void AnimationClip::UpdateInterpolation() {
    if (mKeyframe == nullptr || mNextKeyframe == nullptr) {
        return;
    }
    
    auto& transform = mSceneObject->GetTransform();
    auto& position = mKeyframe->mPosition;
    auto& nextPosition = mNextKeyframe->mPosition;
    if (position.HasValue() && nextPosition.HasValue()) {
        transform.SetPosition(InterpolateVec3(position.GetValue(), nextPosition.GetValue()));
    }

    auto& scale = mKeyframe->mScale;
    auto& nextScale = mNextKeyframe->mScale;
    if (scale.HasValue() && nextScale.HasValue()) {
        transform.SetScale(InterpolateVec3(scale.GetValue(), nextScale.GetValue()));
    }

    auto& rotation = mKeyframe->mRotation;
    auto& nextRotation = mNextKeyframe->mRotation;
    if (rotation.HasValue() && nextRotation.HasValue()) {
        transform.SetRotation(InterpolateVec3(rotation.GetValue(), nextRotation.GetValue()));
    }
}

Pht::Vec3 AnimationClip::InterpolateVec3(const Pht::Vec3& keyframeValue,
                                         const Pht::Vec3& nextKeyframeValue) {
    switch (mInterpolation) {
        case Interpolation::Linear:
            return LerpVec3(keyframeValue, nextKeyframeValue);
        case Interpolation::Cosine:
            return CosineInterpolateVec3(keyframeValue, nextKeyframeValue);
        case Interpolation::None:
            assert(false);
            break;
    }
}

Pht::Vec3 AnimationClip::LerpVec3(const Pht::Vec3& keyframeValue,
                                  const Pht::Vec3& nextKeyframeValue) {
    auto timeBetweenKeyframes = mNextKeyframe->mTime - mKeyframe->mTime;
    auto elapsedInBetweenTime = std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f);
    auto normalizedTime = elapsedInBetweenTime / timeBetweenKeyframes;
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * normalizedTime;
}

Pht::Vec3 AnimationClip::CosineInterpolateVec3(const Pht::Vec3& keyframeValue,
                                               const Pht::Vec3& nextKeyframeValue) {
    auto timeBetweenKeyframes = mNextKeyframe->mTime - mKeyframe->mTime;
    auto elapsedInBetweenTime = std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f);
    auto normalizedTime = elapsedInBetweenTime / timeBetweenKeyframes;
    auto t = std::cos(3.1415f + normalizedTime * 3.1415f) * 0.5f + 0.5f;
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * t;
}

void AnimationClip::Play() {
    assert(mKeyframes.size() >= 2);
    mIsPlaying = true;
}

void AnimationClip::Pause() {
    mIsPlaying = false;
}

void AnimationClip::Stop() {
    mElapsedTime = 0.0f;
    mIsPlaying = false;
    mPreviousKeyframe = nullptr;
    mKeyframe = nullptr;
    mNextKeyframe = nullptr;
}
