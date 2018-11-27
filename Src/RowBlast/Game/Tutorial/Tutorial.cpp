#include "Tutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"
#include "MoveDefinitions.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade {0.5f};
    constexpr auto fadeTime {0.3f};
    const Pht::Vec3 placePieceHandPosition {-2.2f, -3.8f, 0.0f};
    const Pht::Vec3 fillRowsHandPosition {1.5f, -4.3f, 0.0f};
    const Pht::Vec3 switchPieceHandPosition {3.3f, -10.6f, 0.0f};
    const Pht::Vec3 bPieceHandPosition {3.1f, -4.3f, 0.0f};
    const Pht::Vec3 longIPieceHandPosition {0.6f, -7.1f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition1 {3.5f, -7.7f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition2 {1.8f, -8.3f, 0.0f};
    const Pht::Vec3 iPieceHandPosition {-0.8f, -4.5f, 0.0f};
    const Pht::Vec3 secondLevelBPieceHandPosition {-2.5f, -4.4f, 0.0f};
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
    mOtherMovesWindowController {engine, commonResources},
    mCascadingDialogController {engine, commonResources},
    mSameColorDialogController {engine, commonResources},
    mLaserDialogController {engine, commonResources},
    mBombDialogController {engine, commonResources},
    mLevelBombDialogController {engine, commonResources} {
    
    mViewManager.AddView(static_cast<int>(Controller::PlacePieceWindow), mPlacePieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::FillRowsWindow), mFillRowsWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPieceWindow), mSwitchPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::OtherMovesWindow), mOtherMovesWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::CascadingDialog), mCascadingDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SameColorDialog), mSameColorDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::LaserDialog), mLaserDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::BombDialog), mBombDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::LevelBombDialog), mLevelBombDialogController.GetView());

    mCascadingDialogController.SetFadeEffect(mFadeEffect);
    mSameColorDialogController.SetFadeEffect(mFadeEffect);
    mLaserDialogController.SetFadeEffect(mFadeEffect);
    mBombDialogController.SetFadeEffect(mFadeEffect);
    mLevelBombDialogController.SetFadeEffect(mFadeEffect);
}

void Tutorial::Init(const Level& level) {
    mLevel = &level;
    
    if (!level.IsPartOfTutorial()) {
        return;
    }

    mHandAnimation.Init();
    mFadeEffect.Reset();
    
    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());

    mViewManager.Init(uiViewContainer);
    SetActiveViewController(Controller::None);
}

void Tutorial::SetActiveViewController(Controller controller) {
    mActiveViewController = controller;

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

    switch (mActiveViewController) {
        case Controller::PlacePieceWindow:
            if (mPlacePieceWindowController.Update() == PlacePieceWindowController::Result::Done) {
                SetActiveViewController(Controller::FillRowsWindow);
                mFillRowsWindowController.SetUp();
                mHandAnimation.Start(fillRowsHandPosition, 45.0f);
            }
            break;
        case Controller::FillRowsWindow:
            if (mFillRowsWindowController.Update() == FillRowsWindowController::Result::Done) {
                SetActiveViewController(Controller::SwitchPieceWindow);
                mSwitchPieceWindowController.SetUp();
                mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            }
            break;
        case Controller::SwitchPieceWindow:
            if (mSwitchPieceWindowController.Update() == SwitchPieceWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::OtherMovesWindow:
            if (mOtherMovesWindowController.Update() == OtherMovesWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::CascadingDialog:
        case Controller::SameColorDialog:
        case Controller::LaserDialog:
        case Controller::BombDialog:
        case Controller::LevelBombDialog:
        case Controller::None:
            break;
    }
}

Tutorial::Result Tutorial::UpdateDialogs() {
    switch (mActiveViewController) {
        case Controller::CascadingDialog:
            if (mCascadingDialogController.Update() == CascadingDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                return Result::Play;
            }
            break;
        case Controller::SameColorDialog:
            if (mSameColorDialogController.Update() == SameColorDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                return Result::Play;
            }
            break;
        case Controller::LaserDialog:
            if (mLaserDialogController.Update() == LaserDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                return Result::Play;
            }
            break;
        case Controller::BombDialog:
            if (mBombDialogController.Update() == BombDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                return Result::Play;
            }
            break;
        case Controller::LevelBombDialog:
            if (mLevelBombDialogController.Update() == LevelBombDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
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
        case 3:
            SetActiveViewController(Controller::CascadingDialog);
            mCascadingDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 4:
            SetActiveViewController(Controller::SameColorDialog);
            mSameColorDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 6:
            SetActiveViewController(Controller::LaserDialog);
            mLaserDialogController.SetUp(mScene.GetScene());
            return Result::TutorialHasFocus;
        case 7:
            SetActiveViewController(Controller::BombDialog);
            mBombDialogController.SetUp(mScene.GetScene());
            return Result::TutorialHasFocus;
        case 14:
            SetActiveViewController(Controller::LevelBombDialog);
            mLevelBombDialogController.SetUp(mScene.GetScene());
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
        case 2:
            OnNewMoveSecondLevel(numMovesUsedIncludingCurrent);
            break;
        default:
            break;
    }
}

void Tutorial::OnNewMoveFirstLevel(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveViewController(Controller::PlacePieceWindow);
            mPlacePieceWindowController.SetUp();
            mHandAnimation.Start(placePieceHandPosition, -45.0f);
            break;
        case 2:
            mPlacePieceWindowController.Close();
            break;
        case 3:
            mFillRowsWindowController.Close();
            break;
        case 4:
            SetActiveViewController(Controller::SwitchPieceWindow);
            mSwitchPieceWindowController.SetUp();
            mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            break;
        default:
            assert(!"Unsupported number of used moves");
    }
}

void Tutorial::OnNewMoveSecondLevel(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveViewController(Controller::OtherMovesWindow);
            mOtherMovesWindowController.SetUp();
            mHandAnimation.Start(otherMovesHandPosition1, 45.0f);
            break;
        case 2:
            SetActiveViewController(Controller::OtherMovesWindow);
            mOtherMovesWindowController.SetUp();
            mHandAnimation.Start(otherMovesHandPosition2, 90.0f);
            break;
        default:
            break;
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

                if (mActiveViewController == Controller::SwitchPieceWindow) {
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
                        SetActiveViewController(Controller::SwitchPieceWindow);
                        mSwitchPieceWindowController.SetUp();
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

void Tutorial::OnChangeVisibleMoves(int numMovesUsedIncludingCurrent, const Move& firstMove) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

    if (mLevel->GetId() == 2) {
        switch (numMovesUsedIncludingCurrent) {
            case 1:
            case 2: {
                auto& predeterminedMoves {mLevel->GetPredeterminedMoves()};
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove {predeterminedMoves[numMovesUsedIncludingCurrent - 1]};
                
                if (predeterminedMove.mPosition == firstMove.mPosition &&
                    predeterminedMove.mRotation == firstMove.mRotation) {
                    
                    mOtherMovesWindowController.Close();
                    mHandAnimation.Stop();
                    
                    switch (numMovesUsedIncludingCurrent) {
                        case 1:
                            mHandAnimation.Start(iPieceHandPosition, -90.0f);
                            break;
                        case 2:
                            mHandAnimation.Start(secondLevelBPieceHandPosition, 45.0f);
                            break;
                        default:
                            break;
                    }
                } else {
                    SetActiveViewController(Controller::OtherMovesWindow);
                    mOtherMovesWindowController.SetUp();
                    
                    switch (numMovesUsedIncludingCurrent) {
                        case 1:
                            mHandAnimation.Start(otherMovesHandPosition1, 45.0f);
                            break;
                        case 2:
                            mHandAnimation.Start(otherMovesHandPosition2, 45.0f);
                            break;
                        default:
                            break;
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
    
    switch (mLevel->GetId()) {
        case 1:
        case 2:
            if (numMovesUsedIncludingCurrent <= 2) {
                return false;
            }
            break;
        default:
            break;
    }

    return true;
}

bool Tutorial::IsMoveAllowed(int numMovesUsedIncludingCurrent,
                             const Piece& pieceType,
                             const Move& move) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }

    auto& predeterminedMoves {mLevel->GetPredeterminedMoves()};
    
    if (numMovesUsedIncludingCurrent <= predeterminedMoves.size()) {
        auto& predeterminedMove {predeterminedMoves[numMovesUsedIncludingCurrent - 1]};
    
        return predeterminedMove.mPosition == move.mPosition &&
               predeterminedMove.mRotation == move.mRotation &&
               &predeterminedMove.mPieceType == &pieceType;
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
            if (numMovesUsedIncludingCurrent >= 4) {
                return true;
            }
            return false;
        default:
            return true;
    }
}
