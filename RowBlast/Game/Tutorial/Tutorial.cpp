#include "Tutorial.hpp"

#include <assert.h>

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"

using namespace RowBlast;

Tutorial::Tutorial(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources) :
    mScene {scene},
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

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
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

    switch (mActiveController) {
        case Controller::PlacePieceWindow:
            if (mPlacePieceWindowController.Update() == PlacePieceWindowController::Result::Done) {
                SetActiveController(Controller::FillRowsWindow);
                mFillRowsWindowController.Init();
            }
            break;
        case Controller::FillRowsWindow:
            if (mFillRowsWindowController.Update() == FillRowsWindowController::Result::Done) {
                SetActiveController(Controller::SwitchPieceWindow);
                mSwitchPieceWindowController.Init();
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
            mLaserDialogController.Init();
            return Result::TutorialHasFocus;
        case 7:
            SetActiveController(Controller::BombDialog);
            mBombDialogController.Init();
            return Result::TutorialHasFocus;
        default:
            break;
    }

    return Result::Play;
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
            break;
        default:
            assert(!"Unsupported number of used moves");
    }
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
                    }
                } else {
                    if (&predeterminedMove.mPieceType != &pieceType) {
                        SetActiveController(Controller::SwitchPieceWindow);
                        mSwitchPieceWindowController.Init();
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

bool Tutorial::IsPauseAllowed() const {
    return mActiveController == Controller::None;
}
