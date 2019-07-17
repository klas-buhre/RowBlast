#include "Tutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"
#include "MoveDefinitions.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade = 0.5f;
    constexpr auto fadeTime = 0.3f;
    const Pht::Vec3 placePieceHandPosition {-2.2f, -3.8f, 0.0f};
    const Pht::Vec3 fillRowsHandPosition {1.5f, -4.3f, 0.0f};
    const Pht::Vec3 switchPieceHandPosition {1.4f, -10.6f, 0.0f};
    const Pht::Vec3 bPieceHandPosition {-2.5f, -4.4f, 0.0f};
    const Pht::Vec3 longIPieceHandPosition {0.6f, -7.1f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition1 {4.7f, -7.4f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition2 {-1.8f, -7.6f, 0.0f};
    const Pht::Vec3 iPieceHandPosition {-0.8f, -4.5f, 0.0f};
    const Pht::Vec3 secondLevelBPieceHandPosition {-2.5f, -4.4f, 0.0f};
    
    bool FindMove(const ClickInputHandler::VisibleMoves& moves,
                  const Level::TutorialMove& tutorialMove) {
        for (auto& move: moves) {
            if (tutorialMove.mPosition == move.mPosition &&
                tutorialMove.mRotation == move.mRotation) {
                
                return true;
            }
        }
        
        return false;
    }
}

Tutorial::Tutorial(Pht::IEngine& engine,
                   GameScene& scene,
                   const CommonResources& commonResources,
                   const PieceResources& pieceResources,
                   const GhostPieceBlocks& ghostPieceBlocks,
                   const LevelResources& levelResources,
                   const BlastRadiusAnimation& blastRadiusAnimation,
                   const UserServices& userServices) :
    mEngine {engine},
    mScene {scene},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mHandAnimation {engine, 1.0f, true},
    mPlacePieceWindowController {engine, commonResources},
    mFillRowsWindowController {engine, commonResources},
    mSwitchPieceWindowController {engine, commonResources},
    mSwitchPiece2WindowController {engine, commonResources},
    mOtherMovesWindowController {engine, commonResources},
    mOtherMoves2WindowController {engine, commonResources},
    mCascadingDialogController {engine, commonResources},
    mSameColorDialogController {engine, commonResources},
    mLaserDialogController {engine, commonResources, pieceResources, levelResources, userServices},
    mBombDialogController {
        engine,
        commonResources,
        pieceResources,
        levelResources,
        blastRadiusAnimation,
        userServices
    },
    mLevelBombDialogController {
        engine,
        commonResources,
        pieceResources,
        ghostPieceBlocks,
        levelResources,
        userServices
    },
    mAsteroidDialogController {
        engine,
        commonResources,
        pieceResources,
        ghostPieceBlocks,
        levelResources,
        userServices
    } {
    
    mViewManager.AddView(static_cast<int>(Controller::PlacePieceWindow), mPlacePieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::FillRowsWindow), mFillRowsWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPieceWindow), mSwitchPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPiece2Window), mSwitchPiece2WindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::OtherMovesWindow), mOtherMovesWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::OtherMoves2Window), mOtherMoves2WindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::CascadingDialog), mCascadingDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SameColorDialog), mSameColorDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::LaserDialog), mLaserDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::BombDialog), mBombDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::LevelBombDialog), mLevelBombDialogController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::AsteroidDialog), mAsteroidDialogController.GetView());

    mCascadingDialogController.SetFadeEffect(mFadeEffect);
    mSameColorDialogController.SetFadeEffect(mFadeEffect);
    mLaserDialogController.SetFadeEffect(mFadeEffect);
    mBombDialogController.SetFadeEffect(mFadeEffect);
    mLevelBombDialogController.SetFadeEffect(mFadeEffect);
    mAsteroidDialogController.SetFadeEffect(mFadeEffect);
    
    mLaserDialogController.SetGuiLightProvider(scene);
    mBombDialogController.SetGuiLightProvider(scene);
    mLevelBombDialogController.SetGuiLightProvider(scene);
    mAsteroidDialogController.SetGuiLightProvider(scene);
}

void Tutorial::Init(const Level& level) {
    mLevel = &level;
    if (!level.IsPartOfTutorial()) {
        return;
    }

    mHandAnimation.Init(mScene.GetUiViewsContainer());
    mFadeEffect.Reset();
    
    auto& uiViewContainer = mScene.GetUiViewsContainer();
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
                SetActiveViewController(Controller::OtherMovesWindow);
                mOtherMovesWindowController.SetUp();
                mHandAnimation.Start(otherMovesHandPosition1, 270.0f);
            }
            break;
        case Controller::OtherMovesWindow:
            if (mOtherMovesWindowController.Update() == OtherMovesWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::OtherMoves2Window:
            if (mOtherMoves2WindowController.Update() == OtherMoves2WindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::SwitchPieceWindow:
            if (mSwitchPieceWindowController.Update() == SwitchPieceWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::SwitchPiece2Window:
            if (mSwitchPiece2WindowController.Update() == SwitchPiece2WindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::CascadingDialog:
        case Controller::SameColorDialog:
        case Controller::LaserDialog:
        case Controller::BombDialog:
        case Controller::LevelBombDialog:
        case Controller::AsteroidDialog:
        case Controller::None:
            break;
    }
}

Tutorial::Result Tutorial::UpdateDialogs() {
    auto result = Result::TutorialHasFocus;
    
    switch (mActiveViewController) {
        case Controller::CascadingDialog:
            if (mCascadingDialogController.Update() == CascadingDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        case Controller::SameColorDialog:
            if (mSameColorDialogController.Update() == SameColorDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        case Controller::LaserDialog:
            if (mLaserDialogController.Update() == LaserDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        case Controller::BombDialog:
            if (mBombDialogController.Update() == BombDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        case Controller::LevelBombDialog:
            if (mLevelBombDialogController.Update() == LevelBombDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        case Controller::AsteroidDialog:
            if (mAsteroidDialogController.Update() == AsteroidDialogController::Result::Play) {
                SetActiveViewController(Controller::None);
                result = Result::Play;
            }
            break;
        default:
            assert(!"Unsupported dialog");
            break;
    }
    
    if (result == Result::Play) {
        SendAnayticsEvent("Step1Complete");
    }
    
    return result;
}

Tutorial::Result Tutorial::OnLevelStart() {
    if (!mLevel->IsPartOfTutorial()) {
        return Result::Play;
    }

    SendAnayticsEvent("Start");
    
    switch (mLevel->GetId()) {
        case 3:
            SetActiveViewController(Controller::CascadingDialog);
            mCascadingDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 4:
            SetActiveViewController(Controller::SameColorDialog);
            mSameColorDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 5:
            SetActiveViewController(Controller::LaserDialog);
            mLaserDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 7:
            SetActiveViewController(Controller::BombDialog);
            mBombDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 11:
            SetActiveViewController(Controller::LevelBombDialog);
            mLevelBombDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 14:
            SetActiveViewController(Controller::AsteroidDialog);
            mAsteroidDialogController.SetUp();
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
            SendAnayticsEvent("Step1Complete");
            break;
        case 3:
            mFillRowsWindowController.Close();
            SendAnayticsEvent("Step2Complete");
            break;
        case 4:
            SetActiveViewController(Controller::OtherMoves2Window);
            mOtherMoves2WindowController.SetUp();
            mHandAnimation.Start(otherMovesHandPosition2, 45.0f);
            SendAnayticsEvent("Step3Complete");
            break;
        default:
            assert(!"Unsupported number of used moves");
            break;
    }
}

void Tutorial::OnNewMoveSecondLevel(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveViewController(Controller::SwitchPieceWindow);
            mSwitchPieceWindowController.SetUp();
            mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            break;
        case 2:
            SetActiveViewController(Controller::SwitchPiece2Window);
            mSwitchPiece2WindowController.SetUp();
            mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            SendAnayticsEvent("Step1Complete");
            break;
        default:
            break;
    }
}

void Tutorial::OnSelectMove(int numMovesUsedIncludingCurrent) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    switch (mLevel->GetId()) {
        case 1:
            if (numMovesUsedIncludingCurrent == 4) {
                SendAnayticsEvent("Step4Complete");
            }
            break;
        case 2:
            if (numMovesUsedIncludingCurrent == 2) {
                SendAnayticsEvent("Step2Complete");
            }
            break;
    }
    
    mHandAnimation.Stop();
}

void Tutorial::OnSwitchPiece(int numMovesUsedIncludingCurrent, const Piece& pieceType) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    if (mLevel->GetId() == 2) {
        switch (numMovesUsedIncludingCurrent) {
            case 1: {
                auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];

                if (mActiveViewController == Controller::SwitchPieceWindow) {
                    if (&predeterminedMove.mPieceType == &pieceType) {
                        mSwitchPieceWindowController.Close();
                        mHandAnimation.Stop();
                        mHandAnimation.Start(iPieceHandPosition, -90.0f);
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
            case 2: {
                auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];

                if (mActiveViewController == Controller::SwitchPiece2Window) {
                    if (&predeterminedMove.mPieceType == &pieceType) {
                        mSwitchPiece2WindowController.Close();
                        mHandAnimation.Stop();
                        mHandAnimation.Start(secondLevelBPieceHandPosition, 45.0f);
                    }
                } else {
                    if (&predeterminedMove.mPieceType != &pieceType) {
                        SetActiveViewController(Controller::SwitchPiece2Window);
                        mSwitchPiece2WindowController.SetUp();
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

void Tutorial::OnChangeVisibleMoves(int numMovesUsedIncludingCurrent,
                                    const ClickInputHandler::VisibleMoves& visibleMoves) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

    if (mLevel->GetId() == 1) {
        switch (numMovesUsedIncludingCurrent) {
            case 3: {
                auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];
                
                if (FindMove(visibleMoves, predeterminedMove)) {
                    mOtherMovesWindowController.Close();
                    mHandAnimation.Stop();
                    mHandAnimation.Start(bPieceHandPosition, 45.0f);
                } else {
                    SetActiveViewController(Controller::OtherMovesWindow);
                    mOtherMovesWindowController.SetUp();
                    mHandAnimation.Start(otherMovesHandPosition1, 270.0f);
                }
                break;
            }
            case 4: {
                auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
                assert(numMovesUsedIncludingCurrent <= predeterminedMoves.size());
                auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];
                
                if (FindMove(visibleMoves, predeterminedMove)) {
                    mOtherMoves2WindowController.Close();
                    mHandAnimation.Stop();
                    mHandAnimation.Start(longIPieceHandPosition, -90.0f);
                } else {
                    SetActiveViewController(Controller::OtherMoves2Window);
                    mOtherMoves2WindowController.SetUp();
                    mHandAnimation.Start(otherMovesHandPosition2, 45.0f);
                }
                break;
            }
            default:
                break;
        }
    }
}

bool Tutorial::IsSwitchPieceAllowed() const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }
    
    if (mLevel->GetId() == 1) {
        return false;
    }
    
    return true;
}

bool Tutorial::IsSeeMoreMovesAllowed(int numMovesUsedIncludingCurrent) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }
    
    switch (mLevel->GetId()) {
        case 1:
        case 2:
            return (numMovesUsedIncludingCurrent >= 3);
        default:
            return true;
    }
}

bool Tutorial::IsMoveAllowed(int numMovesUsedIncludingCurrent,
                             const Piece& pieceType,
                             const Move& move) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }

    auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
    if (numMovesUsedIncludingCurrent <= predeterminedMoves.size()) {
        auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];
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

void Tutorial::SendAnayticsEvent(const std::string& id) {
    Pht::CustomAnalyticsEvent analyticsEvent {
        "Tutorial:Level" + std::to_string(mLevel->GetId()) + ":" + id
    };
    
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}
