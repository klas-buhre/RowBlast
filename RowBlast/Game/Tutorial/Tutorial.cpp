#include "Tutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
    const Pht::Vec3 placePieceHandPosition {-2.2f, -3.8f, 0.0f};
    const Pht::Vec3 fillRowsHandPosition {1.5f, -4.3f, 0.0f};
    const Pht::Vec3 switchPieceHandPosition {3.3f, -10.6f, 0.0f};
    const Pht::Vec3 bPieceHandPosition {3.1f, -4.3f, 0.0f};
    const Pht::Vec3 longIPieceHandPosition {0.6f, -7.1f, 0.0f};
}

Tutorial::Tutorial(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources) :
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mHandAnimation {engine, scene},
    mPlacePieceWindowController {engine, commonResources},
    mFillRowsWindowController {engine, commonResources},
    mSwitchPieceWindowController {engine, commonResources},
    mLaserDialogController {engine, commonResources},
    mBombDialogController {engine, commonResources} {
    
    mViewManager.AddView(static_cast<int>(Controller::PlacePieceWindow), mPlacePieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::FillRowsWindow), mFillRowsWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPieceWindow), mSwitchPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::LaserDialog), mLaserDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::BombDialog), mBombDialogController.GetView());
}

void Tutorial::Init(const Level& level) {
    mLevel = &level;
    
    if (!level.IsPartOfTutorial()) {
        return;
    }

    mHandAnimation.Init();
    mFadeEffect.Reset();

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    mLaserDialogController.SetFadeEffect(mFadeEffect);
    mBombDialogController.SetFadeEffect(mFadeEffect);
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());

    mViewManager.Init(uiViewContainer);
    SetActiveController(Controller::None);
}

void Tutorial::SetActiveController(Controller controller) {
    mActiveController = controller;

    if (controller == Controller::None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(controller));
    }
}

void Tutorial::Update() {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    mHandAnimation.Update();

    switch (mActiveController) {
        case Controller::PlacePieceWindow:
            if (mPlacePieceWindowController.Update() == PlacePieceWindowController::Result::Done) {
                SetActiveController(Controller::FillRowsWindow);
                mFillRowsWindowController.Init();
                mHandAnimation.Start(fillRowsHandPosition, 45.0f);
            }
            break;
        case Controller::FillRowsWindow:
            if (mFillRowsWindowController.Update() == FillRowsWindowController::Result::Done) {
                SetActiveController(Controller::SwitchPieceWindow);
                mSwitchPieceWindowController.Init();
                mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            }
            break;
        case Controller::SwitchPieceWindow:
            if (mSwitchPieceWindowController.Update() == SwitchPieceWindowController::Result::Done) {
                SetActiveController(Controller::None);
            }
            break;
        case Controller::LaserDialog:
        case Controller::BombDialog:
        case Controller::None:
            break;
    }
}

Tutorial::Result Tutorial::UpdateDialogs() {
    switch (mActiveController) {
        case Controller::LaserDialog:
            if (mLaserDialogController.Update() == LaserDialogController::Result::Play) {
                SetActiveController(Controller::None);
                return Result::Play;
            }
            break;
        case Controller::BombDialog:
            if (mBombDialogController.Update() == BombDialogController::Result::Play) {
                SetActiveController(Controller::None);
                return Result::Play;
            }
            break;
        default:
            assert(!"Unsupported dialog");
    }
    
    return Result::TutorialHasFocus;
}

Tutorial::Result Tutorial::OnLevelStart() {
    switch (mLevel->GetId()) {
        case 6:
            SetActiveController(Controller::LaserDialog);
            mLaserDialogController.Init(mScene.GetScene());
            return Result::TutorialHasFocus;
        case 7:
            SetActiveController(Controller::BombDialog);
            mBombDialogController.Init(mScene.GetScene());
            return Result::TutorialHasFocus;
        default:
            break;
    }

    return Result::Play;
}

void Tutorial::OnPause() {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

    mHandAnimation.Hide();
}

void Tutorial::OnResumePlaying() {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

    mHandAnimation.Unhide();
}

void Tutorial::OnNewMove(int numMovesUsedIncludingCurrent) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    switch (mLevel->GetId()) {
        case 1:
            OnNewMoveFirstLevel(numMovesUsedIncludingCurrent);
            break;
        default:
            break;
    }
}

void Tutorial::OnNewMoveFirstLevel(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveController(Controller::PlacePieceWindow);
            mPlacePieceWindowController.Init();
            mHandAnimation.Start(placePieceHandPosition, -45.0f);
            break;
        case 2:
            mPlacePieceWindowController.Close();
            break;
        case 3:
            mFillRowsWindowController.Close();
            break;
        case 4:
            SetActiveController(Controller::SwitchPieceWindow);
            mSwitchPieceWindowController.Init();
            mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            break;
        default:
            assert(!"Unsupported number of used moves");
    }
}

void Tutorial::OnSelectMove() {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    mHandAnimation.Stop();
}

void Tutorial::OnSwitchPiece(int numMovesUsedIncludingCurrent, const Piece& pieceType) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    if (mLevel->GetId() == 1) {
        switch (numMovesUsedIncludingCurrent) {
            case 3:
            case 4: {
                auto& predeterminedMoves {mLevel->GetPredeterminedMoves()};
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove {predeterminedMoves[numMovesUsedIncludingCurrent - 1]};

                if (mActiveController == Controller::SwitchPieceWindow) {
                    if (&predeterminedMove.mPieceType == &pieceType) {
                        mSwitchPieceWindowController.Close();
                        mHandAnimation.Stop();
                        
                        switch (numMovesUsedIncludingCurrent) {
                            case 3:
                                mHandAnimation.Start(bPieceHandPosition, -45.0f);
                                break;
                            case 4:
                                mHandAnimation.Start(longIPieceHandPosition, -90.0f);
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    if (&predeterminedMove.mPieceType != &pieceType) {
                        SetActiveController(Controller::SwitchPieceWindow);
                        mSwitchPieceWindowController.Init();
                        mHandAnimation.Start(switchPieceHandPosition, -180.0f);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

bool Tutorial::IsSwitchPieceAllowed(int numMovesUsedIncludingCurrent) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }
    
    if (mLevel->GetId() == 1 && numMovesUsedIncludingCurrent <= 2) {
        return false;
    }
    
    return true;
}

bool Tutorial::IsPlacePieceAllowed(int numMovesUsedIncludingCurrent, const Piece& pieceType) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }

    if (mLevel->GetId() == 1) {
        auto& predeterminedMoves {mLevel->GetPredeterminedMoves()};
        assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
        auto& predeterminedMove {predeterminedMoves[numMovesUsedIncludingCurrent - 1]};
        return &predeterminedMove.mPieceType == &pieceType;
    }

    return true;
}

bool Tutorial::IsGestureControlsAllowed() const {
    switch (mLevel->GetId()) {
        case 1:
        case 2:
            return false;
        default:
            return true;
    }
}

bool Tutorial::IsUndoMoveAllowed(int numMovesUsedIncludingCurrent) const {
    switch (mLevel->GetId()) {
        case 1:
            return false;
        case 2:
            if (numMovesUsedIncludingCurrent >= 3) {
                return true;
            }
            return false;
        default:
            return true;
    }
}
