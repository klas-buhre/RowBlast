#ifndef AnimationClip_hpp
#define AnimationClip_hpp

#include <vector>
#include <functional>

#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
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
        
        void SetInterpolation(Interpolation interpolation) {
            mInterpolation = interpolation;
        }
        
        Interpolation GetInterpolation() const {
            return mInterpolation;
        }
        
        void SetWrapMode(WrapMode wrapMode) {
            mWrapMode = wrapMode;
        }
        
        WrapMode GetWrapMode() const {
            return mWrapMode;
        }
        
        const std::vector<Keyframe>& GetKeyframes() const {
            return mKeyframes;
        }
        
    private:
        Interpolation mInterpolation {Interpolation::Linear};
        WrapMode mWrapMode {WrapMode::Loop};
        std::vector<Keyframe> mKeyframes;
    };
}

#endif
