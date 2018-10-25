#ifndef UfoAnimation_hpp
#define UfoAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Ufo;
    
    class UfoAnimation {
    public:
        enum class State {
            WaitingForHighSpeed,
            Active,
            Finished,
            Inactive
        };

        UfoAnimation(Pht::IEngine& engine, Ufo& ufo);
        
        void Init();
        void Start(const Pht::Vec3& destinationPosition);
        void StartHighSpeed(const Pht::Vec3& destinationPosition);
        void StartWarpSpeed(const Pht::Vec3& destinationPosition);
        State Update();
        bool IsActive() const;
        
    private:
        void UpdateRotation();
        void UpdateHoverTranslation();
        void UpdateInWaitingForHighSpeedState();
        void UpdateInActiveState();
        
        Pht::IEngine& mEngine;
        Ufo& mUfo;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mElapsedTiltTime {0.0f};
        float mElapsedHoverTime {0.0f};
        float mAnimationDuration {0.0f};
        Pht::Vec3 mStartPosition;
        Pht::Vec3 mDestinationPosition;
        Pht::Vec3 mRotation;
    };
}

#endif
