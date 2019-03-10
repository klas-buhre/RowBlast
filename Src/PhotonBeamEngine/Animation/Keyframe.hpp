#ifndef Keyframe_hpp
#define Keyframe_hpp

#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
    class Keyframe {
    public:
        Keyframe(float time);
        
        void SetPosition(const Vec3& position);
        void SetScale(const Vec3& scale);
        void SetRotation(const Vec3& rotation);
        void SetIsVisible(bool isVisible);
        
        float GetTime() const {
            return mTime;
        }

        const Optional<Vec3>& GetPosition() const {
            return mPosition;
        }

        const Optional<Vec3>& GetScale() const {
            return mScale;
        }

        const Optional<Vec3>& GetRotation() const {
            return mRotation;
        }
        
        Optional<bool> IsVisible() const {
            return mIsVisible;
        }

    private:
        float mTime {0.0f};
        Optional<Vec3> mPosition;
        Optional<Vec3> mScale;
        Optional<Vec3> mRotation;
        Optional<bool> mIsVisible;
    };
}

#endif
