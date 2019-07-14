#include "ActivePreviewPieceAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 1.0f;
    constexpr auto scaleAmplitude = 0.065f;
}

ActivePreviewPieceAnimation::ActivePreviewPieceAnimation(GameScene& scene, GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}
        
void ActivePreviewPieceAnimation::Init() {
    GoToInactiveState();
}

void ActivePreviewPieceAnimation::Start() {
    mState = State::Active;
    mElapsedTime = 0.0f;
}

void ActivePreviewPieceAnimation::Update(float dt) {
    switch (mGameLogic.GetPreviewPieceAnimationToStart()) {
        case PreviewPieceAnimationToStart::NextPieceAndSwitch:
        case PreviewPieceAnimationToStart::SwitchPiece:
        case PreviewPieceAnimationToStart::RemoveActivePiece:
            GoToInactiveState();
            break;
        case PreviewPieceAnimationToStart::None:
            break;
    }

    switch (mState) {
        case State::Inactive:
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
    }
}

void ActivePreviewPieceAnimation::UpdateInActiveState(float dt) {
    auto* activePreviewPieceSceneObject = mScene.GetHud().GetActivePreviewPieceSceneObject();
    if (activePreviewPieceSceneObject == nullptr) {
        return;
    }
    
    mElapsedTime += dt;
    if (mElapsedTime > animationDuration) {
        mElapsedTime = 0.0f;
    }
    
    auto t = mElapsedTime * 2.0f * 3.1415f / animationDuration;
    auto scale = 1.0f + scaleAmplitude / 2.0f + scaleAmplitude * std::sin(t);
    activePreviewPieceSceneObject->GetTransform().SetScale(scale);
}

void ActivePreviewPieceAnimation::GoToInactiveState() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
}
