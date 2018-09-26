#ifndef UfoAnimation_hpp
#define UfoAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Ufo;
    class MapScene;
    
    class UfoAnimation {
    public:
        enum class State {
            Active,
            Finished,
            Inactive
        };

        UfoAnimation(Pht::IEngine& engine, MapScene& scene, Ufo& ufo);
        
        void Init();
        void Start(const Pht::Vec3& destinationPosition);
        State Update();
        bool IsActive() const;
        
    private:
        void UpdateRotation();
        void UpdateHoverTranslation();
        void UpdateInActiveState();
        
        Pht::IEngine& mEngine;
        MapScene& mScene;
        Ufo& mUfo;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mElapsedTiltTime {0.0f};
        float mElapsedHoverTime {0.0f};
        Pht::Vec3 mStartPosition;
        Pht::Vec3 mDestinationPosition;
        Pht::Vec3 mRotation;
    };
}

#endif
