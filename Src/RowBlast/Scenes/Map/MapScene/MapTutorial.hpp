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
        void OnLevelClick();
        void OnPortalClick();
        void OnUfoAnimationFinished(int worldId);

    private:
        void SendAnalyticsEvent(const std::string& step, const std::string& status);

        enum class State {
            StartLevel2StepStart,
            StartLevel2StepComplete,
            GoToWorld2StepStart,
            GoToWorld2StepComplete,
            Inactive
        };

        Pht::IEngine& mEngine;
        MapScene& mScene;
        const UserServices& mUserServices;
        State mState {State::Inactive};
        HandAnimation mHandAnimation;
    };
}

#endif
