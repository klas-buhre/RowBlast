#ifndef ScrollController_hpp
#define ScrollController_hpp

// Game includes.
#include "Level.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Field;
    class IGameLogic;
    
    class ScrollController {
    public:
        enum class State {
            BeforeLevelOverviewScroll,
            LevelOverviewScroll,
            Idle,
            Scrolling,
            ScrollingSlowly,
            LastScrollStep
        };
        
        enum class ScrollDirection {
            Up,
            Down
        };
        
        ScrollController(Pht::IEngine& engine, Field& field);
        
        void Init(Level::Objective levelObjective);
        State Update();
        void GoToIdleState();
        int CalculatePreferredLowestVisibleRow() const;
        bool IsScrolling() const;
        bool IsScrollingDownInClearMode() const;
        
        float GetLowestVisibleRow() const {
            return mLowestVisibleRow;
        }
        
        State GetState() const {
            return mState;
        }
        
        ScrollDirection GetScrollDirection() const {
            return mScrollDirection;
        }
        
        void SetGameLogic(const IGameLogic& gameLogic) {
            mGameLogic = &gameLogic;
        }
        
    private:
        void UpdateInBeforeLevelOverviewScrollState();
        void UpdateInLevelOverviewScrollState();
        void UpdateInLevelOverviewScrollStateClearObjective(float dt);
        void UpdateInLevelOverviewScrollStateBuildObjective(float dt);
        void UpdateInIdleState();
        int CalculatePreferredLowestVisibleRowClearObjective() const;
        int CalculatePreferredLowestVisibleRowAsteroidObjective() const;
        int CalculatePreferredLowestVisibleRowBuildObjective() const;
        void StartScrollingDown(float preferredLowestVisibleRow);
        void StartScrollingUp(float preferredLowestVisibleRow);
        void UpdateInScrollingState();
        void UpdateInScrollingStateDownDirection(float dt);
        void UpdateInScrollingStateUpDirection(float dt);
        
        Pht::IEngine& mEngine;
        Field& mField;
        const IGameLogic* mGameLogic {nullptr};
        State mState {State::Idle};
        Level::Objective mLevelObjective {Level::Objective::Clear};
        ScrollDirection mScrollDirection {ScrollDirection::Up};
        float mLowestVisibleRow {0.0f};
        float mLowestVisibleRowTarget {0.0f};
        float mScrollSpeed {0.0f};
        float mScrollDeacceleration {0.0f};
        float mScrollTime {0.0f};
        float mWaitedTime {0.0f};
        float mBeforeLevelOverviewScrollWaitTime {0.0f};
        float mStartDeaccelerationTime {0.0f};
    };
}

#endif
