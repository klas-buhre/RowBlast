#ifndef AvatarAnimation_hpp
#define AvatarAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Avatar;
    
    class AvatarAnimation {
    public:
        enum class State {
            Active,
            Inactive
        };

        AvatarAnimation(Pht::IEngine& engine, Avatar& avatar);
        
        void Init();
        void Start(const Pht::Vec3& destinationPosition);
        State Update();
        
    private:
        Pht::IEngine& mEngine;
        Avatar& mAvatar;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        Pht::Vec3 mStartPosition;
        Pht::Vec3 mDestinationPosition;
    };
}

#endif
