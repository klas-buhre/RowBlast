#ifndef Animation_hpp
#define Animation_hpp

#include <unordered_map>

#include "ISceneObjectComponent.hpp"
#include "AnimationClip.hpp"
#include "Vector.hpp"

namespace Pht {
    class SceneObject;
    class IAnimationSystem;
    
    class Animation: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        Animation(SceneObject& sceneObject,
                  const std::vector<Keyframe>& keyframes,
                  IAnimationSystem& animationSystem);
        ~Animation();
        
        void AddClip(const AnimationClip& clip, AnimationClipId clipId);
        void SetActiveClip(AnimationClipId clipId);
        void SetInterpolation(Interpolation interpolation, AnimationClipId clipId = 0);
        AnimationClip* GetClip(AnimationClipId clipId);
        void Update(float dt);
        void Play();
        void Pause();
        void Stop();
        
        bool IsPlaying() const {
            return mIsPlaying;
        }
        
        SceneObject& GetSceneObject() {
            return mSceneObject;
        }

        const SceneObject& GetSceneObject() const {
            return mSceneObject;
        }

    private:
        enum class Action {
            Update,
            Play,
            Pause,
            Stop
        };
        
        void SetKeyframes(const std::vector<Keyframe>& keyframes);
        void CalculateKeyframe(float dt);
        void HandleKeyframeTransition();
        void UpdateInterpolation();
        Pht::Vec3 InterpolateVec3(const Pht::Vec3& keyframeValue,
                                  const Pht::Vec3& nextKeyframeValue);
        Pht::Vec3 LerpVec3(const Pht::Vec3& keyframeValue, const Pht::Vec3& nextKeyframeValue);
        Pht::Vec3 CosineInterpolateVec3(const Pht::Vec3& keyframeValue,
                                        const Pht::Vec3& nextKeyframeValue);
        void PerformActionOnChildAnimations(Action action,
                                            SceneObject& sceneObject,
                                            float dt = 0.0f);
        
        SceneObject& mSceneObject;
        IAnimationSystem& mAnimationSystem;
        AnimationClip* mActiveClip {nullptr};
        std::unordered_map<AnimationClipId, AnimationClip> mClips;
        const Keyframe* mPreviousKeyframe {nullptr};
        const Keyframe* mKeyframe {nullptr};
        const Keyframe* mNextKeyframe {nullptr};
        float mElapsedTime {0.0f};
        bool mIsPlaying {false};
    };
}

#endif
