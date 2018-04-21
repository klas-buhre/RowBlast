#ifndef CameraShake_hpp
#define CameraShake_hpp

#include "Vector.hpp"

namespace Pht {
    class CameraShake {
    public:
        void Init();
        void Update(float dt);
        void StartShake(float shakeTime, float magnitude);

        bool IsShaking() const {
            return mState == State::Shaking;
        }
        
        const Vec3& GetCameraTranslation() const {
            return mCameraTranslation;
        }

    private:
        enum class State {
            Shaking,
            Inactive
        };
        
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mShakeTime {0.0f};
        float mMagnitude {0.0f};
        Vec3 mCameraTranslation;
    };
}

#endif
