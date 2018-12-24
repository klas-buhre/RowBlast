#include "FewMovesAlertAnimation.hpp"

// Engine includes.
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fewMovesAlertLimit {5};
    constexpr auto animationDuration {0.75f};
    constexpr auto animationScaleAmplitude {0.3f};
    constexpr auto animationScaleAdd {0.3f};
}

FewMovesAlertAnimation::FewMovesAlertAnimation(GameScene& scene, const GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

void FewMovesAlertAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
    mMovesTextContainerSceneObject = &mScene.GetHud().GetMovesTextContainer();
}

void FewMovesAlertAnimation::Update(float dt) {
    switch (mState) {
        case State::Inactive:
            UpdateInInactiveState();
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
    }
}

void FewMovesAlertAnimation::UpdateInInactiveState() {
    if (mGameLogic.GetMovesLeft() <= fewMovesAlertLimit) {
        mState = State::Active;
        mElapsedTime = 0.0f;
    }
}

void FewMovesAlertAnimation::UpdateInActiveState(float dt) {
    auto movesLeft {mGameLogic.GetMovesLeft()};
    
    if (movesLeft > fewMovesAlertLimit || movesLeft <= 0) {
        mState = State::Inactive;
        Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextContainerSceneObject,
                                                GameHud::movesTextScale);
    } else {
        mElapsedTime += dt;
        
        if (mElapsedTime > animationDuration) {
            mElapsedTime = 0.0f;
        }
        
        auto t {mElapsedTime * 2.0f * 3.1415f / animationDuration};
        
        auto scale {
            GameHud::movesTextScale + animationScaleAdd + animationScaleAmplitude * std::sin(t)
        };
        
        Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextContainerSceneObject, scale);
    }
}
