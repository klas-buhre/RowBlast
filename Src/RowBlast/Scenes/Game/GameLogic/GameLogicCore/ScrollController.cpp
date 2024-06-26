#include "ScrollController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"

// Game includes.
#include "Field.hpp"
#include "IGameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numVisibleLevelRows = 6;
    constexpr auto numVisibleRowsBelowAsteroid = 7;
    constexpr auto scrollTime = 0.72f;
    constexpr auto deaccelerationStartTime = 0.36f;
    constexpr auto waitTimeClearObjective = 1.0f;
    constexpr auto waitTimeBuildObjective = 0.5f;
    constexpr auto overviewScrollSpeed = 11.0f;
    constexpr auto overviewDeaccelerationDuration = 1.0f;
}

ScrollController::ScrollController(Pht::IEngine& engine, Field& field) :
    mEngine {engine},
    mField {field} {}

void ScrollController::Init(Level::Objective levelObjective) {
    mLevelObjective = levelObjective;
    mState = State::BeforeLevelOverviewScroll;
    mWaitedTime = 0.0f;
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
        case Level::Objective::BringDownTheAsteroid:
            mLowestVisibleRow = 0.0f;
            mBeforeLevelOverviewScrollWaitTime = waitTimeClearObjective;
            break;
        case Level::Objective::Build:
            mLowestVisibleRow = mField.GetNumRows() - mField.GetNumRowsInOneScreen();
            mBeforeLevelOverviewScrollWaitTime = waitTimeBuildObjective;
            break;
    }
}

void ScrollController::GoToIdleState() {
    mState = State::Idle;
}

ScrollController::State ScrollController::Update() {
    switch (mState) {
        case State::BeforeLevelOverviewScroll:
            UpdateInBeforeLevelOverviewScrollState();
            break;
        case State::LevelOverviewScroll:
            UpdateInLevelOverviewScrollState();
            break;
        case State::Idle:
            UpdateInIdleState();
            break;
        case State::Scrolling:
        case State::ScrollingSlowly:
            UpdateInScrollingState();
            break;
        case State::LastScrollStep:
            mState = State::Idle;
            break;
    }
    
    return mState;
}

void ScrollController::UpdateInBeforeLevelOverviewScrollState() {
    mWaitedTime += mEngine.GetLastFrameSeconds();
    
    if (mWaitedTime > mBeforeLevelOverviewScrollWaitTime) {
        mState = State::LevelOverviewScroll;
        mScrollTime = 0.0f;
        mScrollSpeed = overviewScrollSpeed;
        mScrollDeacceleration = mScrollSpeed / overviewDeaccelerationDuration;
        
        switch (mLevelObjective) {
            case Level::Objective::Clear:
            case Level::Objective::BringDownTheAsteroid:
                mLowestVisibleRowTarget = CalculatePreferredLowestVisibleRow();
                mStartDeaccelerationTime =
                    (mLowestVisibleRowTarget - mLowestVisibleRow) / overviewScrollSpeed -
                    overviewDeaccelerationDuration / 2.0f;
                break;
            case Level::Objective::Build:
                mLowestVisibleRowTarget = 0.0f;
                mStartDeaccelerationTime =
                    (mLowestVisibleRow - mLowestVisibleRowTarget) / overviewScrollSpeed -
                    overviewDeaccelerationDuration / 2.0f;
                break;
        }
    }
}

void ScrollController::UpdateInLevelOverviewScrollState() {
    auto dt = mEngine.GetLastFrameSeconds();
    mScrollTime += dt;
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
        case Level::Objective::BringDownTheAsteroid:
            UpdateInLevelOverviewScrollStateClearObjective(dt);
            break;
        case Level::Objective::Build:
            UpdateInLevelOverviewScrollStateBuildObjective(dt);
            break;
    }
}

void ScrollController::UpdateInLevelOverviewScrollStateClearObjective(float dt) {
    mLowestVisibleRow += dt * mScrollSpeed;
    
    if (mScrollTime > mStartDeaccelerationTime) {
        mScrollSpeed -= mScrollDeacceleration * dt;
        if (mScrollSpeed < 0.0f) {
            mScrollSpeed = 0.0f;
        }
    }
    
    if (mLowestVisibleRow >= mLowestVisibleRowTarget || mScrollSpeed == 0.0f) {
        mLowestVisibleRow = mLowestVisibleRowTarget;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRow));
        mState = State::LastScrollStep;
    }
}

void ScrollController::UpdateInLevelOverviewScrollStateBuildObjective(float dt) {
    mLowestVisibleRow -= dt * mScrollSpeed;
    
    if (mScrollTime > mStartDeaccelerationTime) {
        mScrollSpeed -= mScrollDeacceleration * dt;
        if (mScrollSpeed < 0.0f) {
            mScrollSpeed = 0.0f;
        }
    }
    
    if (mLowestVisibleRow <= mLowestVisibleRowTarget || mScrollSpeed == 0.0f) {
        mLowestVisibleRow = mLowestVisibleRowTarget;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRow));
        mState = State::LastScrollStep;
    }
}

void ScrollController::UpdateInIdleState() {
    if (mGameLogic->IsInFieldExplosionsState()) {
        return;
    }

    auto preferredLowestVisibleRow = static_cast<float>(CalculatePreferredLowestVisibleRow());
    if (preferredLowestVisibleRow < mLowestVisibleRow) {
        StartScrollingDown(preferredLowestVisibleRow);
    } else if (preferredLowestVisibleRow > mLowestVisibleRow) {
        StartScrollingUp(preferredLowestVisibleRow);
    }
}

int ScrollController::CalculatePreferredLowestVisibleRow() const {
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            return CalculatePreferredLowestVisibleRowClearObjective();
        case Level::Objective::BringDownTheAsteroid:
            return CalculatePreferredLowestVisibleRowAsteroidObjective();
        case Level::Objective::Build:
            return CalculatePreferredLowestVisibleRowBuildObjective();
    }
}

int ScrollController::CalculatePreferredLowestVisibleRowClearObjective() const {
    auto highestLevelBlock = mField.CalculateHighestLevelBlock();
    auto lowestVisibleRowBasedOnLevelBlocks =
        highestLevelBlock.HasValue() ? highestLevelBlock.GetValue() + 1 - numVisibleLevelRows : 0;

    if (lowestVisibleRowBasedOnLevelBlocks < 0) {
        lowestVisibleRowBasedOnLevelBlocks = 0;
    }
    
    auto lowestVisibleRowBasedOnBlocksInSpawningArea = lowestVisibleRowBasedOnLevelBlocks;
    
    for (;;) {
        auto highestBlockInSpawningArea =
            mField.CalculateHighestBlockInSpawningArea(lowestVisibleRowBasedOnBlocksInSpawningArea);
        
        if (highestBlockInSpawningArea.HasValue()) {
            lowestVisibleRowBasedOnBlocksInSpawningArea = highestBlockInSpawningArea.GetValue() +
                                                          1 - Field::numRowsUpToSpawningArea;
        } else {
            break;
        }
    }
    
    auto highestPossibleLowestVisibleRow = mField.GetNumRows() - mField.GetNumRowsInOneScreen();
    if (lowestVisibleRowBasedOnBlocksInSpawningArea > highestPossibleLowestVisibleRow) {
        lowestVisibleRowBasedOnBlocksInSpawningArea = highestPossibleLowestVisibleRow;
    }
    
    if (lowestVisibleRowBasedOnBlocksInSpawningArea > lowestVisibleRowBasedOnLevelBlocks) {
        return lowestVisibleRowBasedOnBlocksInSpawningArea;
    }
    
    return lowestVisibleRowBasedOnLevelBlocks;
}

int ScrollController::CalculatePreferredLowestVisibleRowAsteroidObjective() const {
    auto asteroidRow = mField.CalculateAsteroidRow();
    auto lowestVisibleRow =
        asteroidRow.HasValue() ? asteroidRow.GetValue() - numVisibleRowsBelowAsteroid : 0;
    
    if (lowestVisibleRow < 0) {
        lowestVisibleRow = 0;
    }
    
    return lowestVisibleRow;
}

int ScrollController::CalculatePreferredLowestVisibleRowBuildObjective() const {
    auto preferredLowestVisibleRow = mField.AccordingToBlueprintHeight();
    auto lowestVisibleRowMax = mField.GetNumRows() - mField.GetNumRowsInOneScreen();
    if (preferredLowestVisibleRow > lowestVisibleRowMax) {
        preferredLowestVisibleRow = lowestVisibleRowMax;
    }
    
    return preferredLowestVisibleRow;
}

void ScrollController::StartScrollingDown(float preferredLowestVisibleRow) {
    mLowestVisibleRowTarget = preferredLowestVisibleRow;
    mScrollSpeed = 2.0f * (mLowestVisibleRow - preferredLowestVisibleRow) /
                   (deaccelerationStartTime + scrollTime);
    mScrollDeacceleration = mScrollSpeed / (scrollTime - deaccelerationStartTime);
    mScrollTime = 0.0f;
    mState = State::Scrolling;
    mScrollDirection = ScrollDirection::Down;
}

void ScrollController::StartScrollingUp(float preferredLowestVisibleRow) {
    mLowestVisibleRowTarget = preferredLowestVisibleRow;
    mScrollSpeed = 2.0f * (preferredLowestVisibleRow - mLowestVisibleRow) /
                   (deaccelerationStartTime + scrollTime);
    mScrollDeacceleration = mScrollSpeed / (scrollTime - deaccelerationStartTime);
    mScrollTime = 0.0f;
    mState = State::Scrolling;
    mScrollDirection = ScrollDirection::Up;
}

void ScrollController::UpdateInScrollingState() {
    auto dt = mEngine.GetLastFrameSeconds();
    mScrollTime += dt;
    
    switch (mScrollDirection) {
        case ScrollDirection::Down:
            UpdateInScrollingStateDownDirection(dt);
            break;
        case ScrollDirection::Up:
            UpdateInScrollingStateUpDirection(dt);
            break;
    }
}

void ScrollController::UpdateInScrollingStateDownDirection(float dt) {
    mLowestVisibleRow -= dt * mScrollSpeed;

    if (mScrollTime > deaccelerationStartTime) {
        mScrollSpeed -= mScrollDeacceleration * dt;
        if (mScrollSpeed < 0.0f) {
            mScrollSpeed = 0.0f;
        }
    }
    
    if (mLowestVisibleRow - mLowestVisibleRowTarget < 0.5f) {
        mState = State::ScrollingSlowly;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRowTarget));
    }
    
    if (mLowestVisibleRow <= mLowestVisibleRowTarget || mScrollTime > scrollTime) {
        mLowestVisibleRow = mLowestVisibleRowTarget;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRow));
        mState = State::LastScrollStep;
    }
}

void ScrollController::UpdateInScrollingStateUpDirection(float dt) {
    mLowestVisibleRow += dt * mScrollSpeed;

    if (mScrollTime > deaccelerationStartTime) {
        mScrollSpeed -= mScrollDeacceleration * dt;
        if (mScrollSpeed < 0.0f) {
            mScrollSpeed = 0.0f;
        }
    }

    if (mLowestVisibleRowTarget - mLowestVisibleRow < 0.5f) {
        mState = State::ScrollingSlowly;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRowTarget));
    }

    if (mLowestVisibleRow >= mLowestVisibleRowTarget || mScrollTime > scrollTime) {
        mLowestVisibleRow = mLowestVisibleRowTarget;
        mField.SetLowestVisibleRow(static_cast<int>(mLowestVisibleRow));
        mState = State::LastScrollStep;
    }
}

bool ScrollController::IsScrolling() const {
    switch (mState) {
        case ScrollController::State::LevelOverviewScroll:
        case ScrollController::State::Scrolling:
        case ScrollController::State::ScrollingSlowly:
        case ScrollController::State::LastScrollStep:
            return true;
        default:
            return false;
    }
}

bool ScrollController::IsScrollingDownInClearMode() const {
    switch (mState) {
        case ScrollController::State::Scrolling:
        case ScrollController::State::ScrollingSlowly:
        case ScrollController::State::LastScrollStep:
            if (mScrollDirection == ScrollDirection::Down &&
                (mLevelObjective == Level::Objective::Clear ||
                 mLevelObjective == Level::Objective::BringDownTheAsteroid)) {
                return true;
            }
            break;
        default:
            break;
    }
    
    return false;
}
