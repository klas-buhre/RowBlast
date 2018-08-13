#include "PreviewPiecesAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {0.28f};
}

PreviewPiecesAnimation::PreviewPiecesAnimation(GameScene& scene, GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

void PreviewPiecesAnimation::Init() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
}

void PreviewPiecesAnimation::Update(float dt) {
    switch (mGameLogic.GetPreviewPieceAnimationToStart()) {
        case PreviewPieceAnimationToStart::NextPieceAndSwitch:
            StartNextPieceAndSwitchingAnimation();
            break;
        case PreviewPieceAnimationToStart::SwitchPiece:
            StartSwitchingPiecesAnimation();
            break;
        case PreviewPieceAnimationToStart::None:
            break;
    }
    
    mGameLogic.ResetPreviewPieceAnimationToStart();
    
    switch (mState) {
        case State::SwitchingPiece: {
            auto normalizedTime {UpdateTime(dt)};
            if (normalizedTime > 1.0f) {
                mState = State::Inactive;
                mScene.GetHud().OnSwitchPieceAnimationFinished();
            } else {
                mSwitchPieceAnimation.Update(normalizedTime);
            }
            break;
        }
        case State::NextPieceAndSwitch: {
            auto normalizedTime {UpdateTime(dt)};
            if (normalizedTime > 1.0f) {
                mState = State::Inactive;
                auto& hud {mScene.GetHud()};
                hud.OnNextPieceAnimationFinished();
                hud.OnSwitchPieceAnimationFinished();
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
    auto& hud {mScene.GetHud()};
    auto& nextPiecesPositionsInHud {hud.GetNextPreviewPiecesRelativePositions()};
    auto& selectablePiecesPositionsInHud {hud.GetSelectablePreviewPiecesRelativePositions()};
    auto& nextPiecesContainerPos {hud.GetNextPiecesContainer().GetTransform().GetPosition()};
    
    auto& selectablePiecesContainerPos {
        hud.GetSelectablePiecesContainer().GetTransform().GetPosition()
    };
    
    Pht::Vec3 nextPieceRightPosition {
        selectablePiecesContainerPos - nextPiecesContainerPos + selectablePiecesPositionsInHud[1]
    };
    
    NextPreviewPiecesPositionsConfig nextPiecesPositions {
        .mLeft = nextPiecesPositionsInHud[0],
        .mSlot1 = nextPiecesPositionsInHud[1],
        .mSlot2 = nextPiecesPositionsInHud[2],
        .mRight = nextPieceRightPosition
    };

    mNextPieceAnimation.StartNextPieceAnimation(hud.GetNextPreviewPieces(), nextPiecesPositions);
    
    SelectablePreviewPiecesPositionsConfig selectablePiecesPositions {
        .mLeft = selectablePiecesPositionsInHud[0],
        .mSlot1 = selectablePiecesPositionsInHud[1],
        .mSlot2 = selectablePiecesPositionsInHud[2],
        .mSlot3 = selectablePiecesPositionsInHud[3],
        .mRight = selectablePiecesPositionsInHud[4]
    };

    mSwitchPieceAnimation.StartSwitchDuringNextPieceAnimation(hud.GetSelectablePreviewPieces(),
                                                              selectablePiecesPositions);
    GoToNextPieceAndSwitchState();
}

void PreviewPiecesAnimation::StartSwitchingPiecesAnimation() {
    auto& hud {mScene.GetHud()};
    auto& piecePositionsInHud {hud.GetSelectablePreviewPiecesRelativePositions()};
    
    SelectablePreviewPiecesPositionsConfig piecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot1 = piecePositionsInHud[1],
        .mSlot2 = piecePositionsInHud[2],
        .mSlot3 = piecePositionsInHud[3],
        .mRight = piecePositionsInHud[4]
    };

    mSwitchPieceAnimation.StartSwitchPieceAnimation(hud.GetSelectablePreviewPieces(),
                                                    piecePositions);
    GoToSwitchingPieceState();
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
