#ifndef MapTutorial_hpp
#define MapTutorial_hpp

// Game includes.
#include "HandAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class UserServices;
    
    class MapTutorial {
    public:
        MapTutorial(Pht::IEngine& engine, MapScene& scene, const UserServices& userServices);
        
        void Init(int worldId);
        void Update();

    private:
        enum class State {
            Active,
            Inactive
        };

        MapScene& mScene;
        const UserServices& mUserServices;
        State mState {State::Inactive};
        HandAnimation mHandAnimation;
    };
}

#endif
