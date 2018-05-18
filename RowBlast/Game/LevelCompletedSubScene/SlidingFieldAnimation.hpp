#ifndef SlidingFieldAnimation_hpp
#define SlidingFieldAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;

    class SlidingFieldAnimation {
    public:
        enum class State {
            Ongoing,
            Inactive
        };

        SlidingFieldAnimation(Pht::IEngine& engine, GameScene& scene);

        void Start();
        State Update();
        
    private:
        void UpdateField();
        void UpdateHud();
        
        State mState {State::Inactive};
        Pht::IEngine& mEngine;
        GameScene& mScene;
        float mElapsedTime {0.0f};
        Pht::Vec3 mFieldInitialPosition;
        Pht::Vec3 mFieldFinalPosition;
        Pht::Vec2 mScissorBoxRelativePosition;
        float mUpperHudInitialYPosition {0.0f};
        float mUpperHudFinalYPosition {0.0f};
        float mLowerHudInitialYPosition {0.0f};
        float mLowerHudFinalYPosition {0.0f};
    };
}

#endif
