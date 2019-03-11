#include "Animation.hpp"

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
    
    mElapsedTime += dt;
    
    PerformActionOnChildAnimations(Action::Update, mSceneObject, dt);
}

void Animation::Play() {
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
