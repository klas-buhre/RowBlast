#include "PreviewPiecesAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"
#include "ActivePreviewPieceAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 0.28f;
}

PreviewPiecesAnimation::PreviewPiecesAnimation(GameScene& scene,
                                               GameLogic& gameLogic,
                                               ActivePreviewPieceAnimation& activePreviewPieceAnimation) :
    mScene {scene},
    mGameLogic {gameLogic},
    mActivePreviewPieceAnimation {activePreviewPieceAnimation} {}

void PreviewPiecesAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
    mNextPieceAnimation.Init();
    mSwitchPieceAnimation.Init();
}

void PreviewPiecesAnimation::Update(float dt) {
    switch (mGameLogic.GetPreviewPieceAnimationToStart()) {
        case PreviewPieceAnimationToStart::NextPieceAndSwitch:
        case PreviewPieceAnimationToStart::NextPieceAndRefillActive:
            StartNextPieceAndSwitchingAnimation();
            break;
        case PreviewPieceAnimationToStart::NextPieceAndRefillSelectable0:
            StartNextPieceAndRefillSelectable0Animation();
            break;
        case PreviewPieceAnimationToStart::NextPieceAndRefillSelectable1:
            StartNextPieceAndRefillSelectable1Animation();
            break;
        case PreviewPieceAnimationToStart::SwitchPiece:
            StartSwitchingPiecesAnimation();
            break;
        case PreviewPieceAnimationToStart::RemoveActivePiece:
            StartRemoveActivePieceAnimation();
            return;
        case PreviewPieceAnimationToStart::None:
            break;
    }

    switch (mState) {
        case State::SwitchingPiece:
        case State::RemovingActivePiece: {
            auto normalizedTime = UpdateTime(dt);
            if (normalizedTime > 1.0f) {
                mState = State::Inactive;
                mScene.GetHud().OnSwitchPieceAnimationFinished();
                mActivePreviewPieceAnimation.Start();
            } else {
                mSwitchPieceAnimation.Update(normalizedTime);
            }
            break;
        }
        case State::NextPieceAndSwitch: {
            auto normalizedTime = UpdateTime(dt);
            if (normalizedTime > 1.0f) {
                mState = State::Inactive;
                auto& hud = mScene.GetHud();
                hud.OnNextPieceAnimationFinished();
                hud.OnSwitchPieceAnimationFinished();
                mActivePreviewPieceAnimation.Start();
            } else {
                mNextPieceAnimation.Update(normalizedTime);
                mSwitchPieceAnimation.Update(normalizedTime);
            }
            break;
        }
        case State::Inactive:
            break;
    }
}

void PreviewPiecesAnimation::StartNextPieceAndSwitchingAnimation() {
    StartNextPieceAnimation();
    
    auto& hud = mScene.GetHud();
    auto& selectablePiecesPositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();

    SelectablePreviewPiecesPositionsConfig selectablePiecesPositions {
        .mLeft = selectablePiecesPositionsInHud[0],
        .mSlot0 = selectablePiecesPositionsInHud[1],
        .mSlot1 = selectablePiecesPositionsInHud[2],
        .mSlot2 = selectablePiecesPositionsInHud[3],
        .mRight = selectablePiecesPositionsInHud[4]
    };

    mSwitchPieceAnimation.StartSwitchDuringNextPieceAnimation(hud.GetSelectablePreviewPieces(),
                                                              selectablePiecesPositions);
    GoToNextPieceAndSwitchState();
}

void PreviewPiecesAnimation::StartNextPieceAnimation() {
    auto& hud = mScene.GetHud();
    auto& nextPiecesPositionsInHud = hud.GetNextPreviewPiecesRelativePositions();
    auto& selectablePiecesPositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();
    auto& nextContainerTransform = hud.GetNextPiecesContainer().GetTransform();
    auto& nextContainerPos = nextContainerTransform.GetPosition();
    auto& nextSceneObjectTransform = hud.GetNextPiecesSceneObject().GetTransform();
    auto nextSceneObjectScale = nextSceneObjectTransform.GetScale().x;
    auto& selectablesContainerTransform = hud.GetSelectablePiecesContainer().GetTransform();
    auto& selectablesContainerPos = selectablesContainerTransform.GetPosition();
    auto& selectablesSceneObjectTransform = hud.GetSelectablePiecesSceneObject().GetTransform();
    auto selectablesSceneObjectScale = selectablesSceneObjectTransform.GetScale().x;
    
    Pht::Vec3 nextPiecePositionInSelectables {
        (selectablesContainerPos - nextContainerPos +
         selectablePiecesPositionsInHud[3] * selectablesSceneObjectScale) / nextSceneObjectScale
    };
    
    NextPreviewPiecesPositionsConfig nextPiecesPositions {
        .mRightSelectable = nextPiecePositionInSelectables,
        .mSlot0 = nextPiecesPositionsInHud[0],
        .mSlot1 = nextPiecesPositionsInHud[1],
        .mLower = nextPiecesPositionsInHud[2]
    };
    
    mNextPieceAnimation.StartNextPieceAnimation(hud.GetNextPreviewPieces(),
                                                nextPiecesPositions,
                                                selectablesSceneObjectScale / nextSceneObjectScale);
}

void PreviewPiecesAnimation::StartNextPieceAndRefillSelectable0Animation() {
    StartNextPieceAnimation();
    
    auto& hud = mScene.GetHud();
    auto& selectablePiecesPositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();
    
    SelectablePreviewPiecesPositionsConfig selectablePiecesPositions {
        .mLeft = selectablePiecesPositionsInHud[0],
        .mSlot0 = selectablePiecesPositionsInHud[1],
        .mSlot1 = selectablePiecesPositionsInHud[2],
        .mSlot2 = selectablePiecesPositionsInHud[3],
        .mRight = selectablePiecesPositionsInHud[4]
    };
    
    mSwitchPieceAnimation.StartRefillSelectable0Animation(hud.GetSelectablePreviewPieces(),
                                                          selectablePiecesPositions);
    GoToNextPieceAndSwitchState();
}

void PreviewPiecesAnimation::StartNextPieceAndRefillSelectable1Animation() {
    StartNextPieceAnimation();
    mSwitchPieceAnimation.StartRefillSelectable1Animation();
    GoToNextPieceAndSwitchState();
}

void PreviewPiecesAnimation::StartSwitchingPiecesAnimation() {
    auto& hud = mScene.GetHud();
    auto& piecePositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();
    
    SelectablePreviewPiecesPositionsConfig piecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot0 = piecePositionsInHud[1],
        .mSlot1 = piecePositionsInHud[2],
        .mSlot2 = piecePositionsInHud[3],
        .mRight = piecePositionsInHud[4]
    };

    mSwitchPieceAnimation.StartSwitchPieceAnimation(hud.GetSelectablePreviewPieces(),
                                                    piecePositions);
    GoToSwitchingPieceState();
}

void PreviewPiecesAnimation::StartRemoveActivePieceAnimation() {
    auto& hud = mScene.GetHud();
    auto& piecePositionsInHud = hud.GetSelectablePreviewPiecesRelativePositions();

    SelectablePreviewPiecesPositionsConfig piecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot0 = piecePositionsInHud[1],
        .mSlot1 = piecePositionsInHud[2],
        .mSlot2 = piecePositionsInHud[3],
        .mRight = piecePositionsInHud[4]
    };

    mSwitchPieceAnimation.StartRemoveActivePieceAnimation(hud.GetSelectablePreviewPieces(),
                                                          piecePositions);
    GoToRemovingActivePieceState();
}

float PreviewPiecesAnimation::UpdateTime(float dt) {
    mElapsedTime += dt;
    return mElapsedTime / animationDuration;
}

void PreviewPiecesAnimation::GoToSwitchingPieceState() {
    if (mState == State::NextPieceAndSwitch) {
        mScene.GetHud().OnNextPieceAnimationFinished();
    }
    
    mState = State::SwitchingPiece;
    mElapsedTime = 0.0f;
}

void PreviewPiecesAnimation::GoToNextPieceAndSwitchState() {
    mState = State::NextPieceAndSwitch;
    mElapsedTime = 0.0f;
}

void PreviewPiecesAnimation::GoToRemovingActivePieceState() {
    if (mState == State::NextPieceAndSwitch) {
        mScene.GetHud().OnNextPieceAnimationFinished();
    }
    
    mState = State::RemovingActivePiece;
    mElapsedTime = 0.0f;
}
