#ifndef AnimationClip_hpp
#define AnimationClip_hpp

#include <vector>
#include <functional>

#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
    class SceneObject;

    enum class Interpolation {
        None,
        Linear,
        Cosine
    };
    
    enum class WrapMode {
        Once,
        Loop
    };

    struct Keyframe {
        float mTime {0.0f};
        Optional<Vec3> mPosition;
        Optional<Vec3> mScale;
        Optional<Vec3> mRotation;
        Optional<bool> mIsVisible;
        std::function<void()> mCallback;
    };

    using AnimationClipId = uint32_t;
    
    class AnimationClip {
    public:
        explicit AnimationClip(const std::vector<Keyframe>& keyframes);

        void Update(float dt);
        void Play();
        void Pause();
        void Stop();

        void SetInterpolation(Interpolation interpolation) {
            mInterpolation = interpolation;
        }
        
        void SetWrapMode(WrapMode wrapMode) {
            mWrapMode = wrapMode;
        }
                
        bool IsPlaying() const {
            return mIsPlaying;
        }
        
    private:
        friend class Animation;
        
        bool CalculateKeyframe(float dt);
        void HandleKeyframeTransition();
        void UpdateInterpolation();
        Pht::Vec3 InterpolateVec3(const Pht::Vec3& keyframeValue,
                                  const Pht::Vec3& nextKeyframeValue);
        Pht::Vec3 LerpVec3(const Pht::Vec3& keyframeValue, const Pht::Vec3& nextKeyframeValue);
        Pht::Vec3 CosineInterpolateVec3(const Pht::Vec3& keyframeValue,
                                        const Pht::Vec3& nextKeyframeValue);

        SceneObject* mSceneObject {nullptr};
        Interpolation mInterpolation {Interpolation::Linear};
        WrapMode mWrapMode {WrapMode::Loop};
        std::vector<Keyframe> mKeyframes;
        const Keyframe* mPreviousKeyframe {nullptr};
        const Keyframe* mKeyframe {nullptr};
        const Keyframe* mNextKeyframe {nullptr};
        float mElapsedTime {0.0f};
        bool mIsPlaying {false};
    };
}

#endif
