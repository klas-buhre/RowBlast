#ifndef PortalCameraMovement_hpp
#define PortalCameraMovement_hpp

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;

    class PortalCameraMovement {
    public:
        PortalCameraMovement(Pht::IEngine& engine, MapScene& scene);
        
        enum class State {
            Inactive,
            Active,
            StartFadeOut
        };
        
        void Start();
        State Update();
        
    private:
        void UpdateInActiveState();
        
        Pht::IEngine& mEngine;
        MapScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mStartZ {0.0f};
        bool mHasBeenInStartFadeOutState {false};
    };
}

#endif
