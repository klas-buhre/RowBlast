#include "Animation.hpp"

#include "AnimationSystem.hpp"
#include "Fnv1Hash.hpp"

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
}

void Animation::Play() {
    mIsPlaying = true;
}

void Animation::Pause() {
    mIsPlaying = false;
}

void Animation::Stop() {
    mElapsedTime = 0.0f;
    mIsPlaying = false;
}
