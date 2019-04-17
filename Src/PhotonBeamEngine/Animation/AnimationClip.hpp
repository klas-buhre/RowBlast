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
        
        const std::vector<Keyframe>& GetKeyframes() const {
            return mKeyframes;
        }
        
    private:
        Interpolation mInterpolation {Interpolation::Linear};
        std::vector<Keyframe> mKeyframes;
    };
}

#endif
