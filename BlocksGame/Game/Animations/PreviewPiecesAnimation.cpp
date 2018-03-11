#include "PreviewPiecesAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto animationDuration {0.28f};
}

PreviewPiecesAnimation::PreviewPiecesAnimation(GameScene& scene, GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

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
    auto& piecePositionsInHud {hud.GetPreviewPieceRelativePositions()};
    auto& nextPiecesContainerPos {hud.GetNextPiecesContainer().GetTransform().GetPosition()};
    
    auto& selectablePiecesContainerPos {
        hud.GetSelectablePiecesContainer().GetTransform().GetPosition()
    };
    
    Pht::Vec3 nextPieceRightPosition {
        selectablePiecesContainerPos - nextPiecesContainerPos + piecePositionsInHud[1]
    };
    
    PreviewPiecePositionsConfig nextPiecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot1 = piecePositionsInHud[1],
        .mSlot2 = piecePositionsInHud[2],
        .mRight = nextPieceRightPosition
    };

    mNextPieceAnimation.Start(PreviewPieceGroupAnimation::Kind::NextPiece,
                              hud.GetNextPreviewPieces(),
                              nextPiecePositions);
    
    PreviewPiecePositionsConfig switchPiecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot1 = piecePositionsInHud[1],
        .mSlot2 = piecePositionsInHud[2],
        .mRight = piecePositionsInHud[3]
    };

    mSwitchPieceAnimation.Start(PreviewPieceGroupAnimation::Kind::SwitchDuringNextPiece,
                                hud.GetSelectablePreviewPieces(),
                                switchPiecePositions);
    GoToNextPieceAndSwitchState();
}

void PreviewPiecesAnimation::StartSwitchingPiecesAnimation() {
    auto& hud {mScene.GetHud()};
    auto& piecePositionsInHud {hud.GetPreviewPieceRelativePositions()};
    
    PreviewPiecePositionsConfig piecePositions {
        .mLeft = piecePositionsInHud[0],
        .mSlot1 = piecePositionsInHud[1],
        .mSlot2 = piecePositionsInHud[2],
        .mRight = piecePositionsInHud[3]
    };

    mSwitchPieceAnimation.Start(PreviewPieceGroupAnimation::Kind::Switch,
                                hud.GetSelectablePreviewPieces(),
                                piecePositions);
    GoToSwitchingPieceState();
}

float PreviewPiecesAnimation::UpdateTime(float dt) {
    mElapsedTime += dt;
    return mElapsedTime / animationDuration;
}

void PreviewPiecesAnimation::GoToSwitchingPieceState() {
    mState = State::SwitchingPiece;
    mElapsedTime = 0.0f;
}

void PreviewPiecesAnimation::GoToNextPieceAndSwitchState() {
    mState = State::NextPieceAndSwitch;
    mElapsedTime = 0.0f;
}
