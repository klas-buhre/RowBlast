#include "FewMovesAlertAnimation.hpp"

// Engine includes.
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fewMovesAlertLimit = 4;
    constexpr auto animationDuration = 0.75f;
    constexpr auto animationScaleAmplitude = 0.06f;
    constexpr auto animationScaleAdd = 0.06f;
    constexpr auto animationTextScaleAmplitude = 0.41f;
    constexpr auto animationTextScaleAdd = 0.41f;
}

FewMovesAlertAnimation::FewMovesAlertAnimation(GameScene& scene, const GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

void FewMovesAlertAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
    
    auto& hud = mScene.GetHud();
    mMovesRoundedCylinderContainer = &hud.GetMovesRoundedCylinderContainer();
    mMovesTextContainerSceneObject = &hud.GetMovesTextContainer();
    mMovesTextSceneObject = &hud.GetMovesTextSceneObject();
    
    hud.ShowBlueMovesIcon();
}

void FewMovesAlertAnimation::Update(float dt) {
    switch (mState) {
        case State::Inactive:
            UpdateInInactiveState();
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::ZeroMoves:
            UpdateInZeroMovesState();
            break;
    }
}

void FewMovesAlertAnimation::UpdateInInactiveState() {
    if (mGameLogic.GetMovesLeft() <= fewMovesAlertLimit) {
        mState = State::Active;
        mElapsedTime = 0.0f;
        mScene.GetHud().ShowYellowMovesIcon();
    }
}

void FewMovesAlertAnimation::UpdateInActiveState(float dt) {
    auto movesLeft = mGameLogic.GetMovesLeft();
    if (movesLeft > fewMovesAlertLimit) {
        mState = State::Inactive;
        RestoreHud();
    } else if (movesLeft <= 0) {
        mState = State::ZeroMoves;
        RestoreHud();
    } else {
        mElapsedTime += dt;
        if (mElapsedTime > animationDuration) {
            mElapsedTime = 0.0f;
        }
        
        auto t = mElapsedTime * 2.0f * 3.1415f / animationDuration;
        auto sinT = std::sin(t);

        auto movesContainerScale = 1.0f + animationScaleAdd + animationScaleAmplitude * sinT;
        mMovesRoundedCylinderContainer->GetTransform().SetScale(movesContainerScale);
        
        auto textContainerScale = 1.0f + animationTextScaleAdd + animationTextScaleAmplitude * sinT;
        mMovesTextContainerSceneObject->GetTransform().SetScale(textContainerScale);
        
        auto textScale =
            GameHud::movesContainerScale + animationTextScaleAdd +
            animationTextScaleAmplitude * sinT;
        auto fontSizeAdjustedTextScale = textScale * mScene.GetHud().GetMovesTextScaleFactor();
        Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextSceneObject, fontSizeAdjustedTextScale);
    }
}

void FewMovesAlertAnimation::UpdateInZeroMovesState() {
    if (mGameLogic.GetMovesLeft() > fewMovesAlertLimit) {
        mState = State::Inactive;
        RestoreHud();
    }
}

void FewMovesAlertAnimation::RestoreHud() {
    mScene.GetHud().ShowBlueMovesIcon();
    mMovesRoundedCylinderContainer->GetTransform().SetScale(1.0f);
    mMovesTextContainerSceneObject->GetTransform().SetScale(GameHud::movesTextStaticScale);
    
    auto textScale =
        GameHud::movesContainerScale * GameHud::movesTextStaticScale *
        mScene.GetHud().GetMovesTextScaleFactor();
    Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextSceneObject, textScale);
}
