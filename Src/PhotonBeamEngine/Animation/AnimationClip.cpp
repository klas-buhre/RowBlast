#include "AnimationClip.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "SceneObjectUtils.hpp"

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

    auto textScale = newKeyframe.mTextScale;
    if (textScale.HasValue()) {
        SceneObjectUtils::ScaleRecursively(*mSceneObject, textScale.GetValue());
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
        transform.SetPosition(Interpolate(position.GetValue(), nextPosition.GetValue()));
    }

    auto& scale = mKeyframe->mScale;
    auto& nextScale = mNextKeyframe->mScale;
    if (scale.HasValue() && nextScale.HasValue()) {
        transform.SetScale(Interpolate(scale.GetValue(), nextScale.GetValue()));
    }

    auto& rotation = mKeyframe->mRotation;
    auto& nextRotation = mNextKeyframe->mRotation;
    if (rotation.HasValue() && nextRotation.HasValue()) {
        transform.SetRotation(Interpolate(rotation.GetValue(), nextRotation.GetValue()));
    }
    
    auto textScale = mKeyframe->mTextScale;
    auto nextTextScale = mNextKeyframe->mTextScale;
    if (textScale.HasValue() && nextTextScale.HasValue()) {
        auto scale = Interpolate(textScale.GetValue(), nextTextScale.GetValue());
        SceneObjectUtils::ScaleRecursively(*mSceneObject, scale);
    }
}

template<typename T>
T AnimationClip::Interpolate(const T& keyframeValue, const T& nextKeyframeValue) {
    switch (mInterpolation) {
        case Interpolation::Linear:
            return Lerp(keyframeValue, nextKeyframeValue);
        case Interpolation::Cosine:
            return CosineInterpolate(keyframeValue, nextKeyframeValue);
        case Interpolation::None:
            assert(false);
            break;
    }
}

template<typename T>
T AnimationClip::Lerp(const T& keyframeValue, const T& nextKeyframeValue) {
    auto timeBetweenKeyframes = mNextKeyframe->mTime - mKeyframe->mTime;
    auto elapsedInBetweenTime = std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f);
    auto normalizedTime = elapsedInBetweenTime / timeBetweenKeyframes;
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * normalizedTime;
}

template<typename T>
T AnimationClip::CosineInterpolate(const T& keyframeValue, const T& nextKeyframeValue) {
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
