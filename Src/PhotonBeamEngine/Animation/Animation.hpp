#ifndef Animation_hpp
#define Animation_hpp

#include <vector>

#include "Keyframe.hpp"
#include "ISceneObjectComponent.hpp"

namespace Pht {
    class SceneObject;
    class IAnimationSystem;

    enum class Interpolation {
        None,
        Linear
    };
    
    class Animation: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        Animation(SceneObject& sceneObject, IAnimationSystem& animationSystem);
        ~Animation();
        
        void AddKeyframe(const Keyframe& keyframe);
        void Update(float dt);
        void Play();
        void Pause();
        void Stop();
        
        void SetInterpolation(Interpolation interpolation) {
            mInterpolation = interpolation;
        }
        
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
        
        void CalculateKeyframe(float dt);
        void HandleKeyframeTransition();
        void UpdateLinearInterpolation();
        Pht::Vec3 LerpVec3(const Pht::Vec3& keyframeValue, const Pht::Vec3& nextKeyframeValue);
        void PerformActionOnChildAnimations(Action action,
                                            SceneObject& sceneObject,
                                            float dt = 0.0f);
        
        SceneObject& mSceneObject;
        IAnimationSystem& mAnimationSystem;
        Interpolation mInterpolation {Interpolation::Linear};
        std::vector<Keyframe> mKeyframes;
        Keyframe* mPreviousKeyframe {nullptr};
        Keyframe* mKeyframe {nullptr};
        Keyframe* mNextKeyframe {nullptr};
        float mElapsedTime {0.0f};
        bool mIsPlaying {false};
    };
}

#endif
