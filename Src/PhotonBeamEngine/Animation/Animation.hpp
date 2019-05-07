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
        
        void AddClip(AnimationClip clip, AnimationClipId clipId);
        void SetDefaultClip(AnimationClipId clipId);
        void SetInterpolation(Interpolation interpolation, AnimationClipId clipId = 0);
        AnimationClip* GetClip(AnimationClipId clipId);
        void Update(float dt);
        void Play();
        void Play(AnimationClipId clipId);
        void Pause();
        void Stop();
        bool IsPlaying() const;

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
        
        void PerformActionOnChildAnimations(Action action,
                                            SceneObject& sceneObject,
                                            float dt = 0.0f);
        
        SceneObject& mSceneObject;
        IAnimationSystem& mAnimationSystem;
        std::unordered_map<AnimationClipId, AnimationClip> mClips;
        AnimationClip* mDefaultClip {nullptr};
    };
}

#endif
