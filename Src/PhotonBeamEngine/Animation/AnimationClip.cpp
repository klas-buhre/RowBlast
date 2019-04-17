#include "AnimationClip.hpp"

#include <assert.h>

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
}
