#include "FinalScoreAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countingDownMovesDuration = 0.75f;
    constexpr auto scoreTextWaitDuration = 0.35f;
}

FinalScoreAnimation::FinalScoreAnimation(Pht::IEngine& engine,
                                         GameLogic& gameLogic,
                                         GameScene& scene,
                                         const CommonResources& commonResources) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mScene {scene},
    mScoreTexts {scene, commonResources, ScoreTexts::SceneId::LevelCompletedSubScene} {}

void FinalScoreAnimation::Init() {
    mState = State::Inactive;
    mScoreTexts.Init();
}

void FinalScoreAnimation::Start() {
    if (mState != State::Inactive) {
        return;
    }

    auto numMovesLeft = mGameLogic.GetMovesLeft();
    if (numMovesLeft == 0) {
        return;
    }

    mElapsedTime = 0.0f;
    mBonusPoints = mGameLogic.CalculateBonusPointsAtLevelCompleted();
    mScoreAtLevelCompleted = mGameLogic.GetScore();
    mState = State::MovesCountdown;
    auto& hud = mScene.GetHud();
    hud.SetNumMovesLeftSource(GameHud::NumMovesLeftSource::Self);
    hud.SetNumMovesLeft(numMovesLeft);
}

void FinalScoreAnimation::Update() {
    switch (mState) {
        case State::MovesCountdown:
            UpdateInMovesCountdownState();
            break;
        case State::MovesCountdownAndScoreText:
            UpdateInMovesCountdownAndScoreTextState();
            break;
        case State::Inactive:
        case State::Done:
            break;
    }
}

void FinalScoreAnimation::UpdateInMovesCountdownState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    UpdateMovesCountdown();
    if (mElapsedTime > scoreTextWaitDuration) {
        mState = State::MovesCountdownAndScoreText;
        mScoreTexts.Start(mBonusPoints, {-0.4f, -1.325f}, 0.0f);
    }
}

void FinalScoreAnimation::UpdateInMovesCountdownAndScoreTextState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    UpdateMovesCountdown();
    mScoreTexts.Update(mEngine.GetLastFrameSeconds());
    if (mScoreTexts.IsInactive() && mElapsedTime > countingDownMovesDuration) {
        mState = State::Done;
    }
}

void FinalScoreAnimation::UpdateMovesCountdown() {
    if (mElapsedTime < countingDownMovesDuration) {
        auto numMovesLeftInCountdown =
            static_cast<int>(static_cast<float>(mGameLogic.GetMovesLeft()) *
            (countingDownMovesDuration - mElapsedTime) / countingDownMovesDuration);
        mScene.GetHud().SetNumMovesLeft(numMovesLeftInCountdown);
        
        auto score =
            mScoreAtLevelCompleted +
            static_cast<int>(static_cast<float>(mBonusPoints) * mElapsedTime / countingDownMovesDuration);
        mGameLogic.SetScore(score);
    } else {
        mScene.GetHud().SetNumMovesLeft(0);
        mGameLogic.SetScore(mScoreAtLevelCompleted + mBonusPoints);
    }
}
