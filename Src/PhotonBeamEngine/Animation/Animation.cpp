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
    
    mClips.emplace(0, keyframes);
    SetActiveClip(0);
}

Animation::~Animation() {
    mAnimationSystem.RemoveAnimation(*this);
}

void Animation::AddClip(const AnimationClip& clip, AnimationClipId clipId) {
    mClips.insert(std::make_pair(clipId, clip));
}

void Animation::SetActiveClip(AnimationClipId clipId) {
    auto* clip = GetClip(clipId);
    assert(clip);
    mActiveClip = clip;
}

void Animation::SetInterpolation(Interpolation interpolation, AnimationClipId clipId) {
    auto* clip = GetClip(clipId);
    assert(clip);
    clip->SetInterpolation(interpolation);
}

AnimationClip* Animation::GetClip(AnimationClipId clipId) {
    auto i = mClips.find(clipId);
    if (i != std::end(mClips)) {
        return &i->second;
    }

    return nullptr;
}

void Animation::Update(float dt) {
    if (!mIsPlaying) {
        return;
    }
    
    CalculateKeyframe(dt);
    if (mKeyframe != mPreviousKeyframe) {
        HandleKeyframeTransition();
    }
    
    if (mActiveClip->GetInterpolation() != Interpolation::None) {
        UpdateInterpolation();
    }
    
    PerformActionOnChildAnimations(Action::Update, mSceneObject, dt);
    
    mPreviousKeyframe = mKeyframe;
}

void Animation::CalculateKeyframe(float dt) {
    mElapsedTime += dt;
    
    auto& keyframes = mActiveClip->GetKeyframes();
    
    for (auto i = 1; i < keyframes.size(); ++i) {
        auto& keyframe = keyframes[i - 1];
        auto& nextKeyframe = keyframes[i];
        if (mElapsedTime >= keyframe.mTime && mElapsedTime <= nextKeyframe.mTime) {
            mKeyframe = &keyframe;
            mNextKeyframe = &nextKeyframe;
            return;
        }
    }
    
    mElapsedTime = 0.0f;
    mKeyframe = &keyframes[0];
    mNextKeyframe = &keyframes[1];
}

void Animation::HandleKeyframeTransition() {
    if (mKeyframe == nullptr) {
        return;
    }
    
    auto& transform = mSceneObject.GetTransform();
    auto& position = mKeyframe->mPosition;
    if (position.HasValue()) {
        transform.SetPosition(position.GetValue());
    }
    
    auto& scale = mKeyframe->mScale;
    if (scale.HasValue()) {
        transform.SetScale(scale.GetValue());
    }

    auto& rotation = mKeyframe->mRotation;
    if (rotation.HasValue()) {
        transform.SetRotation(rotation.GetValue());
    }

    auto isVisible = mKeyframe->mIsVisible;
    if (isVisible.HasValue()) {
        mSceneObject.SetIsVisible(isVisible.GetValue());
    }
    
    auto& callback = mKeyframe->mCallback;
    if (callback) {
        callback();
    }
}

void Animation::UpdateInterpolation() {
    if (mKeyframe == nullptr || mNextKeyframe == nullptr) {
        return;
    }
    
    auto& transform = mSceneObject.GetTransform();
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

Pht::Vec3 Animation::InterpolateVec3(const Pht::Vec3& keyframeValue,
                                     const Pht::Vec3& nextKeyframeValue) {
    switch (mActiveClip->GetInterpolation()) {
        case Interpolation::Linear:
            return LerpVec3(keyframeValue, nextKeyframeValue);
        case Interpolation::Cosine:
            return CosineInterpolateVec3(keyframeValue, nextKeyframeValue);
        case Interpolation::None:
            assert(false);
            break;
    }
}

Pht::Vec3 Animation::LerpVec3(const Pht::Vec3& keyframeValue, const Pht::Vec3& nextKeyframeValue) {
    auto timeBetweenKeyframes = mNextKeyframe->mTime - mKeyframe->mTime;
    auto elapsedInBetweenTime = std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f);
    auto normalizedTime = elapsedInBetweenTime / timeBetweenKeyframes;
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * normalizedTime;
}

Pht::Vec3 Animation::CosineInterpolateVec3(const Pht::Vec3& keyframeValue,
                                           const Pht::Vec3& nextKeyframeValue) {
    auto timeBetweenKeyframes = mNextKeyframe->mTime - mKeyframe->mTime;
    auto elapsedInBetweenTime = std::fmax(mElapsedTime - mKeyframe->mTime, 0.0f);
    auto normalizedTime = elapsedInBetweenTime / timeBetweenKeyframes;
    auto t = std::cos(3.1415f + normalizedTime * 3.1415f) * 0.5f + 0.5f;
    
    return keyframeValue + (nextKeyframeValue - keyframeValue) * t;
}

void Animation::Play() {
    assert(mActiveClip->GetKeyframes().size() >= 2);

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
        auto* childAnimation = childSceneObject->GetComponent<Animation>();
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
