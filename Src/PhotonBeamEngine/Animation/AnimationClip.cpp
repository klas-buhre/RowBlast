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
    
    Rewind();
}

void AnimationClip::Update(float dt) {
    if (!mIsPlaying) {
        return;
    }
    
    if (mElapsedTime == 0.0f) {
        HandleKeyframeTransition(mKeyframes.front());
    }
    
    mElapsedTime += dt;
    if (mElapsedTime >= mNextKeyframe->mTime) {
        if (!CalculateKeyframe()) {
            switch (mWrapMode) {
                case WrapMode::Once:
                    Stop();
                    return;
                case WrapMode::Loop:
                    Rewind();
                    break;
            }
        }
    }
    
    if (mInterpolation != Interpolation::None) {
        UpdateInterpolation();
    }
}

bool AnimationClip::CalculateKeyframe() {
    for (auto i = 0; i < mKeyframes.size() - 1; ++i) {
        auto& keyframe = mKeyframes[i];
        auto& nextKeyframe = mKeyframes[i + 1];
        if (mElapsedTime >= keyframe.mTime && mElapsedTime <= nextKeyframe.mTime) {
            for (auto j = mKeyframeIndex + 1; j <= i; ++j) {
                HandleKeyframeTransition(mKeyframes[j]);
            }
            
            mKeyframe = &keyframe;
            mNextKeyframe = &nextKeyframe;
            mKeyframeIndex = i;
            return true;
        }
    }
    
    for (auto j = mKeyframeIndex + 1; j < mKeyframes.size(); ++j) {
        HandleKeyframeTransition(mKeyframes[j]);
    }
    
    return false;
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
    mIsPlaying = false;
    Rewind();
}

void AnimationClip::Rewind() {
    assert(mKeyframes.size() >= 2);
    mElapsedTime = 0.0f;
    mKeyframeIndex = 0;
    mKeyframe = &mKeyframes[0];
    mNextKeyframe = &mKeyframes[1];
}
