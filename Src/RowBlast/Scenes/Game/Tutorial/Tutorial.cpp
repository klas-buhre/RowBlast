#include "Tutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IAnalytics.hpp"
#include "IAnimationSystem.hpp"
#include "AnalyticsEvent.hpp"
#include "Scene.hpp"

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"
#include "MoveDefinitions.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fade = 0.5f;
    constexpr auto fadeTime = 0.3f;

    const Pht::Vec3 placePieceHandPosition {-2.8f, -3.86f, 0.0f};
    const Pht::Vec3 fillRowsHandPosition {1.5f, -3.46f, 0.0f};
    const Pht::Vec3 switchPieceHandPosition {1.4f, -9.76f, 0.0f};
    const Pht::Vec3 mirroredSevenPieceHandPosition {-1.6f, -2.76f, 0.0f};
    const Pht::Vec3 longIPieceHandPosition {0.3f, -6.46f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition1 {4.5f, -6.76f, 0.0f};
    const Pht::Vec3 otherMovesHandPosition2 {-1.8f, -7.76f, 0.0f};
    const Pht::Vec3 iPieceHandPosition {-1.1f, -3.86f, 0.0f};
    const Pht::Vec3 secondLevelBPieceHandPosition {-2.5f, -3.56f, 0.0f};

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
    
    bool MovesAreTheSame(const Move& move,
                         const Level::TutorialMove& predeterminedMove,
                         const Piece& pieceType) {
        if (&predeterminedMove.mPieceType != &pieceType) {
            return false;
        }

        if (predeterminedMove.mPosition == move.mPosition &&
            predeterminedMove.mRotation == move.mRotation) {
            
            return true;
        }

        auto& duplicateMoveCheck = pieceType.GetDuplicateMoveCheck(move.mRotation);
        if (duplicateMoveCheck.HasValue()) {
            auto& duplicateMoveCheckValue = duplicateMoveCheck.GetValue();
            if (predeterminedMove.mPosition == move.mPosition + duplicateMoveCheckValue.mRelativePosition &&
                predeterminedMove.mRotation == duplicateMoveCheckValue.mRotation) {
                
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
                   const BlastArea& blastRadiusAnimation,
                   const UserServices& userServices) :
    mEngine {engine},
    mScene {scene},
    mUserServices {userServices},
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        fadeTime,
        fade,
        UiLayer::backgroundFade
    },
    mHandAnimation {engine, 1.4f, true},
    mPlacePieceWindowController {
        engine,
        commonResources,
        {
            "You play by putting pieces in smart",
            "places. Tap the move to the left",
            "to put the blue piece in that spot"
        },
        5.4f
    },
    mPlacePieceHintWindowController {
        engine,
        commonResources,
        {
            "The move to the left would be a",
            "better choice. Tap the left move to",
            "place the blue piece in that spot"
        },
        5.4f
    },
    mFillRowsWindowController {
        engine,
        commonResources,
        {
            "Blocks are cleared when",
            "horizontal rows are filled. Tap the",
            "suggested move to clear three rows"
        },
        5.4f
    },
    mFillRowsHintWindowController {
        engine,
        commonResources,
        {
            "The move in the middle is the best",
            "choice since it will fill three rows.",
            "Tap that move to clear three rows"
        },
        5.4f
    },
    mSwitchPieceWindowController {
        engine,
        commonResources,
        {
            "Try switching the current piece.",
            "The current piece is the left one of",
            "the three small pieces at the bottom"
        },
        4.3f
    },
    mSwitchPieceHintWindowController {
        engine,
        commonResources,
        {
            "There could be a better fitting",
            "piece. Try a different piece by",
            "tapping the switch button instead"
        },
        4.3f
    },
    mPlaceIPieceWindowController {
        engine,
        commonResources,
        {"This piece fits much better.", "Tap the move in the middle" },
        5.4f
    },
    mSwitchPiece2WindowController {
        engine,
        commonResources,
        {"Let's try a different piece by", "tapping the switch button again"},
        5.4f
    },
    mSwitchPiece3WindowController {
        engine,
        commonResources,
        {
            "This piece fits OK but let's try the",
            "next piece by tapping the switch",
            "button one more time"
        },
        5.4f
    },
    mPlaceBPieceWindowController {
        engine,
        commonResources,
        {"This piece is clearly the best", "choice. Tap the move to the left" },
        5.4f
    },
    mOtherMovesWindowController {
        engine,
        commonResources,
        {
            "You can always find more moves",
            "by tapping the screen. Tap",
            "anywhere except buttons and moves"
        },
        5.4f
    },
    mPlaceMirroredSevenPieceWindowController {
        engine,
        commonResources,
        {
            "Now you have found a good move!",
            "Tap the move to the left to clear",
            "a couple of rows"
        },
        5.4f
    },
    mOtherMoves2WindowController {
        engine,
        commonResources,
        {
            "Tap the screen again to find",
            "a better move. Remember not to",
            "press any moves or buttons"
        },
        6.7f
    },
    mPlaceYellowPieceHintWindowController {
        engine,
        commonResources,
        {
            "Tap the move at the bottom",
            "right to clear five rows!"
        },
        6.7f
    },
    mRotateBluePieceWindowController {
        engine,
        commonResources,
        {
            "Tap the blue piece to rotate it.",
        },
        4.0f
    },
    mDragBluePieceWindowController {
        engine,
        commonResources,
        {
            "You play by putting pieces in",
            "smart places. Drag the blue",
            "piece into the playing field."
        },
        4.0f
    },
    mDragGreenPieceWindowController {
        engine,
        commonResources,
        {
            "Blocks are cleared when",
            "horizontal rows are filled. Place",
            "the green piece to clear three rows."
        },
        4.0f
    },
    mRotateGreenPieceWindowController {
        engine,
        commonResources,
        {
            "Tap the green piece to rotate it.",
        },
        4.0f
    },
    mPlayOnYourOwnWindowController {engine, commonResources},
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
    mViewManager.AddView(static_cast<int>(Controller::PlacePieceHintWindow), mPlacePieceHintWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::FillRowsWindow), mFillRowsWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::FillRowsHintWindow), mFillRowsHintWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPieceWindow), mSwitchPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPieceHintWindow), mSwitchPieceHintWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::PlaceIPieceWindow), mPlaceIPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPiece2Window), mSwitchPiece2WindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::SwitchPiece3Window), mSwitchPiece3WindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::PlaceBPieceWindow), mPlaceBPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::OtherMovesWindow), mOtherMovesWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::PlaceMirroredSevenPieceWindow), mPlaceMirroredSevenPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::OtherMoves2Window), mOtherMoves2WindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::PlaceYellowPieceHintWindow), mPlaceYellowPieceHintWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::PlayOnYourOwnWindow), mPlayOnYourOwnWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::RotateBluePieceWindow), mRotateBluePieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::DragBluePieceWindow), mDragBluePieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::DragGreenPieceWindow), mDragGreenPieceWindowController.GetView());
    mViewManager.AddView(static_cast<int>(Controller::RotateGreenPieceWindow), mRotateGreenPieceWindowController.GetView());
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
    mDragAndDropAnimations.clear();
    
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    mHandAnimation.Init(mScene.GetUiViewsContainer());
    mFadeEffect.Reset();
    
    mPlayOnYourOwnWindowController.Init();
    
    auto& uiViewContainer = mScene.GetUiViewsContainer();
    uiViewContainer.AddChild(mFadeEffect.GetSceneObject());

    mViewManager.Init(uiViewContainer);
    SetActiveViewController(Controller::None);
    
    if (level.GetId() == 0) {
        InitDragAndDropTutorial();
    }
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
    if (!IsLevelPartOfTutorial()) {
        return;
    }
    
    mHandAnimation.Update();
    
    if (mLevel->GetId() == 0) {
        for (auto& dragAndDropAnimation: mDragAndDropAnimations) {
            dragAndDropAnimation->mAnimation->Update(mEngine.GetLastFrameSeconds());
            dragAndDropAnimation->mHandAnimation.Update();
        }
    }

    switch (mActiveViewController) {
        case Controller::PlacePieceWindow:
            if (mPlacePieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::FillRowsWindow);
                mFillRowsWindowController.SetUp();
                mHandAnimation.Start(fillRowsHandPosition, 45.0f);
            }
            break;
        case Controller::PlacePieceHintWindow:
            if (mPlacePieceHintWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::FillRowsWindow);
                mFillRowsWindowController.SetUp();
                mHandAnimation.Start(fillRowsHandPosition, 45.0f);
            }
            break;
        case Controller::FillRowsWindow:
            if (mFillRowsWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::OtherMovesWindow);
                mOtherMovesWindowController.SetUp();
                mHandAnimation.Start(otherMovesHandPosition1, 270.0f);
            }
            break;
        case Controller::FillRowsHintWindow:
            if (mFillRowsHintWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::OtherMovesWindow);
                mOtherMovesWindowController.SetUp();
                mHandAnimation.Start(otherMovesHandPosition1, 270.0f);
            }
            break;
        case Controller::OtherMovesWindow:
            if (mOtherMovesWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::PlaceMirroredSevenPieceWindow:
            if (mPlaceMirroredSevenPieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::OtherMoves2Window);
                mOtherMoves2WindowController.SetUp();
                mHandAnimation.Start(otherMovesHandPosition2, 45.0f);
            }
            break;
        case Controller::OtherMoves2Window:
            if (mOtherMoves2WindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::PlaceYellowPieceHintWindow:
            if (mPlaceYellowPieceHintWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::SwitchPieceWindow:
            if (mSwitchPieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::SwitchPieceHintWindow:
            if (mSwitchPieceHintWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::PlaceIPieceWindow:
            if (mPlaceIPieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::SwitchPiece2Window);
                mSwitchPiece2WindowController.SetUp();
                mHandAnimation.Start(switchPieceHandPosition, -180.0f);
            }
            break;
        case Controller::SwitchPiece2Window:
            if (mSwitchPiece2WindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::SwitchPiece3Window:
            if (mSwitchPiece3WindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::PlaceBPieceWindow:
            if (mPlaceBPieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::PlayOnYourOwnWindow);
                mPlayOnYourOwnWindowController.SetUp();
            }
            break;
        case Controller::PlayOnYourOwnWindow:
            if (mPlayOnYourOwnWindowController.Update() == PlayOnYourOwnWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::RotateBluePieceWindow:
            if (mRotateBluePieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::DragBluePieceWindow);
                mDragBluePieceWindowController.SetUp();
            }
            break;
        case Controller::DragBluePieceWindow:
            if (mDragBluePieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::DragGreenPieceWindow:
            if (mDragGreenPieceWindowController.Update() == TutorialWindowController::Result::Done) {
                SetActiveViewController(Controller::None);
            }
            break;
        case Controller::RotateGreenPieceWindow:
            if (mRotateGreenPieceWindowController.Update() == TutorialWindowController::Result::Done) {
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
    if (!IsLevelPartOfTutorial()) {
        return Result::Play;
    }

    SendAnayticsEvent("Start");
    
    switch (mLevel->GetId()) {
        case 3:
            SetActiveViewController(Controller::CascadingDialog);
            mCascadingDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 4:
            if (ShouldDisplayAnimationWindow()) {
                SetActiveViewController(Controller::LaserDialog);
                mLaserDialogController.SetUp();
                return Result::TutorialHasFocus;
            }
            break;
        case 5:
            SetActiveViewController(Controller::SameColorDialog);
            mSameColorDialogController.SetUp();
            return Result::TutorialHasFocus;
        case 7:
            if (ShouldDisplayAnimationWindow()) {
                SetActiveViewController(Controller::BombDialog);
                mBombDialogController.SetUp();
                return Result::TutorialHasFocus;
            }
            break;
        case 11:
            if (ShouldDisplayAnimationWindow()) {
                SetActiveViewController(Controller::LevelBombDialog);
                mLevelBombDialogController.SetUp();
                return Result::TutorialHasFocus;
            }
            break;
        case 14:
            if (ShouldDisplayAnimationWindow()) {
                SetActiveViewController(Controller::AsteroidDialog);
                mAsteroidDialogController.SetUp();
                return Result::TutorialHasFocus;
            }
            break;
        default:
            break;
    }

    return Result::Play;
}

void Tutorial::OnPause() {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    mHandAnimation.Hide();
    mPlayOnYourOwnWindowController.OnPause();
}

void Tutorial::OnResumePlaying() {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    mHandAnimation.Unhide();
    mPlayOnYourOwnWindowController.OnResumePlaying();
}

void Tutorial::OnBeginDragPiece() {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    SetActiveViewController(Controller::None);
    StopDragAndDropAnimations();
}

void Tutorial::OnDragPieceEnd(int numMovesUsedIncludingCurrent, Rotation rotation) {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    if (mLevel->GetId() == 0) {
        switch (rotation) {
            case Rotation::Deg0:
            case Rotation::Deg180:
                switch (numMovesUsedIncludingCurrent) {
                    case 1:
                        StartTapToRotateAnimation({-2.1f, -11.0f, UiLayer::root});
                        break;
                    case 2:
                        StartDragAndDropAnimation(numMovesUsedIncludingCurrent - 1);
                        break;
                    case 3:
                        StartTapToRotateAnimation({0.9f, -11.0f, UiLayer::root});
                        break;
                    default:
                        break;
                }
                break;
            case Rotation::Deg90:
            case Rotation::Deg270:
                switch (numMovesUsedIncludingCurrent) {
                    case 1:
                    case 2:
                    case 3:
                        StartDragAndDropAnimation(numMovesUsedIncludingCurrent - 1);
                        break;
                    default:
                        break;
                }
                break;
        }
    }
}

void Tutorial::OnRotateActivePreviewPiece(int numMovesUsedIncludingCurrent, Rotation rotation) {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    if (mLevel->GetId() == 0 && numMovesUsedIncludingCurrent == 1) {
        OnRotatePreviewPiece(PreviewPieceIndex::Active,
                             numMovesUsedIncludingCurrent,
                             rotation,
                             {-2.1f, -11.0f, UiLayer::root});
    }
}

void Tutorial::OnRotateSelectable0PreviewPiece(int numMovesUsedIncludingCurrent,
                                               Rotation rotation) {
    if (!IsLevelPartOfTutorial()) {
        return;
    }

    if (mLevel->GetId() == 0 && numMovesUsedIncludingCurrent == 3) {
        OnRotatePreviewPiece(PreviewPieceIndex::Selectable0,
                             numMovesUsedIncludingCurrent,
                             rotation,
                             {0.9f, -11.0f, UiLayer::root});
    }
}

void Tutorial::OnRotatePreviewPiece(PreviewPieceIndex previewPieceIndex,
                                    int numMovesUsedIncludingCurrent,
                                    Rotation rotation,
                                    const Pht::Vec3& handPosition) {
    switch (rotation) {
        case Rotation::Deg0:
        case Rotation::Deg180:
            switch (previewPieceIndex) {
                case PreviewPieceIndex::Active:
                    SetActiveViewController(Controller::RotateBluePieceWindow);
                    mRotateBluePieceWindowController.SetUp();
                    break;
                case PreviewPieceIndex::Selectable0:
                    SetActiveViewController(Controller::RotateGreenPieceWindow);
                    mRotateGreenPieceWindowController.SetUp();
                    break;
                default:
                    break;
            }
            StopDragAndDropAnimations();
            StartTapToRotateAnimation(handPosition);
            break;
        case Rotation::Deg90:
        case Rotation::Deg270:
            switch (previewPieceIndex) {
                case PreviewPieceIndex::Active:
                    mRotateBluePieceWindowController.Close();
                    break;
                case PreviewPieceIndex::Selectable0:
                    mRotateGreenPieceWindowController.Close();
                    break;
                default:
                    break;
            }
            mHandAnimation.Stop();
            StartDragAndDropAnimation(numMovesUsedIncludingCurrent - 1);
            break;
        default:
            break;
    }
}

void Tutorial::OnNewMove(int numMovesUsedIncludingCurrent) {
    if (!IsLevelPartOfTutorial()) {
        return;
    }
    
    switch (mLevel->GetId()) {
        case 0:
            OnNewMoveDragAndDropTutorial(numMovesUsedIncludingCurrent);
            break;
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

void Tutorial::OnNewMoveDragAndDropTutorial(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveViewController(Controller::RotateBluePieceWindow);
            mRotateBluePieceWindowController.SetUp();
            StartTapToRotateAnimation({-2.1f, -11.0f, UiLayer::root});
            break;
        case 2:
            SetActiveViewController(Controller::DragGreenPieceWindow);
            mDragGreenPieceWindowController.SetUp();
            StartDragAndDropAnimation(numMovesUsedIncludingCurrent - 1);
            break;
        case 3:
            SetActiveViewController(Controller::RotateGreenPieceWindow);
            mRotateGreenPieceWindowController.SetUp();
            StartTapToRotateAnimation({0.9f, -11.0f, UiLayer::root});
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
            if (mActiveViewController == Controller::PlacePieceWindow) {
                mPlacePieceWindowController.Close();
            } else {
                mPlacePieceHintWindowController.Close();
            }
            SendAnayticsEvent("Step1Complete");
            break;
        case 3:
            if (mActiveViewController == Controller::FillRowsWindow) {
                mFillRowsWindowController.Close();
            } else {
                mFillRowsHintWindowController.Close();
            }
            SendAnayticsEvent("Step2Complete");
            break;
        case 4:
            mPlaceMirroredSevenPieceWindowController.Close();
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
            mPlaceIPieceWindowController.Close();
            SendAnayticsEvent("Step1Complete");
            break;
        case 3:
            mPlaceBPieceWindowController.Close();
        case 4:
            mPlayOnYourOwnWindowController.Close();
            break;
        default:
            break;
    }
}

void Tutorial::OnSelectMove(int numMovesUsedIncludingCurrent) {
    if (!IsLevelPartOfTutorial()) {
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
    if (!IsLevelPartOfTutorial()) {
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
                        SetActiveViewController(Controller::PlaceIPieceWindow);
                        mPlaceIPieceWindowController.SetUp();
                        mHandAnimation.Stop();
                        mHandAnimation.Start(iPieceHandPosition, -90.0f);
                    }
                } else if (mActiveViewController == Controller::SwitchPieceHintWindow) {
                    if (&predeterminedMove.mPieceType == &pieceType) {
                        mSwitchPieceHintWindowController.Close();
                        SetActiveViewController(Controller::PlaceIPieceWindow);
                        mPlaceIPieceWindowController.SetUp();
                        mHandAnimation.Stop();
                        mHandAnimation.Start(iPieceHandPosition, -90.0f);
                    } else {
                        mSwitchPieceHintWindowController.Close();
                        SetActiveViewController(Controller::SwitchPieceWindow);
                        mSwitchPieceWindowController.SetUp();
                        mHandAnimation.Start(switchPieceHandPosition, -180.0f);
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

                if (mActiveViewController == Controller::SwitchPiece3Window) {
                    if (&predeterminedMove.mPieceType == &pieceType) {
                        mSwitchPiece3WindowController.Close();
                        SetActiveViewController(Controller::PlaceBPieceWindow);
                        mPlaceBPieceWindowController.SetUp();
                        mHandAnimation.Stop();
                        mHandAnimation.Start(secondLevelBPieceHandPosition, 45.0f);
                    }
                } else if (mActiveViewController == Controller::SwitchPiece2Window) {
                    if (&predeterminedMove.mPieceType != &pieceType) {
                        SetActiveViewController(Controller::SwitchPiece3Window);
                        mSwitchPiece3WindowController.SetUp();
                        mHandAnimation.Start(switchPieceHandPosition, -180.0f);
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
    if (!IsLevelPartOfTutorial()) {
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
                    SetActiveViewController(Controller::PlaceMirroredSevenPieceWindow);
                    mPlaceMirroredSevenPieceWindowController.SetUp();
                    mHandAnimation.Stop();
                    mHandAnimation.Start(mirroredSevenPieceHandPosition, 45.0f);
                } else {
                    mPlaceMirroredSevenPieceWindowController.Close();
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
                    SetActiveViewController(Controller::PlaceYellowPieceHintWindow);
                    mPlaceYellowPieceHintWindowController.SetUp();
                    mHandAnimation.Stop();
                    mHandAnimation.Start(longIPieceHandPosition, -90.0f);
                } else {
                    mPlaceYellowPieceHintWindowController.Close();
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

bool Tutorial::IsRotatePreviewPieceAllowed(int numMovesUsedIncludingCurrent,
                                           PreviewPieceIndex previewPieceIndex) const {
    if (!IsLevelPartOfTutorial()) {
        return true;
    }

    if (mLevel->GetId() == 0) {
        switch (numMovesUsedIncludingCurrent) {
            case 1:
                if (previewPieceIndex == PreviewPieceIndex::Active) {
                    return true;
                } else {
                    return false;
                }
                break;
            case 2:
                return false;
            case 3:
                if (previewPieceIndex == PreviewPieceIndex::Selectable0) {
                    return true;
                } else {
                    return false;
                }
                break;
            default:
                return true;
        }
    }
    
    return true;
}

bool Tutorial::IsDragPieceAllowed(int numMovesUsedIncludingCurrent,
                                  PreviewPieceIndex draggedPieceIndex,
                                  Rotation rotation) const {
    if (!IsLevelPartOfTutorial()) {
        return true;
    }

    if (mLevel->GetId() == 0) {
        switch (numMovesUsedIncludingCurrent) {
            case 1:
                if (draggedPieceIndex == PreviewPieceIndex::Active) {
                    switch (rotation) {
                        case Rotation::Deg0:
                        case Rotation::Deg180:
                            return false;
                        case Rotation::Deg90:
                        case Rotation::Deg270:
                            return true;
                    }
                } else {
                    return false;
                }
                break;
            case 2:
                if (draggedPieceIndex == PreviewPieceIndex::Selectable0) {
                    return true;
                } else {
                    return false;
                }
                break;
            case 3:
                if (draggedPieceIndex == PreviewPieceIndex::Selectable0) {
                    switch (rotation) {
                        case Rotation::Deg0:
                        case Rotation::Deg180:
                            return false;
                        case Rotation::Deg90:
                        case Rotation::Deg270:
                            return true;
                    }
                } else {
                    return false;
                }
                break;
            default:
                return true;
        }
    }

    return true;
}

bool Tutorial::IsSwitchPieceAllowed() const {
    if (!IsLevelPartOfTutorial()) {
        return true;
    }
    
    if (mLevel->GetId() == 1) {
        return false;
    }
    
    return true;
}

bool Tutorial::IsSeeMoreMovesAllowed(int numMovesUsedIncludingCurrent) const {
    if (!IsLevelPartOfTutorial()) {
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
                             const Move& move) {
    if (!IsLevelPartOfTutorial()) {
        return true;
    }

    auto& predeterminedMoves = mLevel->GetPredeterminedMoves();
    if (numMovesUsedIncludingCurrent <= predeterminedMoves.size()) {
        auto& predeterminedMove = predeterminedMoves[numMovesUsedIncludingCurrent - 1];
        auto isMoveAllowed = MovesAreTheSame(move, predeterminedMove, pieceType);
        if (!isMoveAllowed) {
            switch (mActiveViewController) {
                case Controller::PlacePieceWindow:
                    mPlacePieceWindowController.Close();
                    SetActiveViewController(Controller::PlacePieceHintWindow);
                    mPlacePieceHintWindowController.SetUp();
                    break;
                case Controller::PlacePieceHintWindow:
                    mPlacePieceHintWindowController.SetUp();
                    break;
                case Controller::FillRowsWindow:
                    mFillRowsWindowController.Close();
                    SetActiveViewController(Controller::FillRowsHintWindow);
                    mFillRowsHintWindowController.SetUp();
                    break;
                case Controller::FillRowsHintWindow:
                    mFillRowsHintWindowController.SetUp();
                    break;
                case Controller::OtherMovesWindow:
                    mOtherMovesWindowController.SetUp();
                    break;
                case Controller::PlaceMirroredSevenPieceWindow:
                    mPlaceMirroredSevenPieceWindowController.SetUp();
                    break;
                case Controller::OtherMoves2Window:
                    mOtherMoves2WindowController.SetUp();
                    break;
                case Controller::PlaceYellowPieceHintWindow:
                    mPlaceYellowPieceHintWindowController.SetUp();
                    break;
                case Controller::SwitchPieceWindow:
                    mSwitchPieceWindowController.Close();
                    SetActiveViewController(Controller::SwitchPieceHintWindow);
                    mSwitchPieceHintWindowController.SetUp();
                    break;
                case Controller::SwitchPieceHintWindow:
                    mSwitchPieceHintWindowController.SetUp();
                    break;
                case Controller::PlaceIPieceWindow:
                    mPlaceIPieceWindowController.SetUp();
                    break;
                case Controller::SwitchPiece2Window:
                    mSwitchPiece2WindowController.SetUp();
                    break;
                case Controller::SwitchPiece3Window:
                    mSwitchPiece3WindowController.SetUp();
                    break;
                case Controller::PlaceBPieceWindow:
                    mPlaceBPieceWindowController.SetUp();
                    break;
                default:
                    break;
            }
        } else {
            if (mActiveViewController == Controller::PlaceYellowPieceHintWindow) {
                mPlaceYellowPieceHintWindowController.Close();
            }
        }
        
        return isMoveAllowed;
    }

    return true;
}

bool Tutorial::IsControlTypeChangeAllowed() const {
    switch (mLevel->GetId()) {
        case 0:
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

bool Tutorial::IsPauseAllowed() const {
    switch (mLevel->GetId()) {
        case 0:
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

bool Tutorial::IsLevelPartOfTutorial() const {
    switch (mLevel->GetId()) {
        case 0:
        case 1:
            return true;
        case 2:
            return mUserServices.GetSettingsService().GetControlType() == ControlType::Click;
        default:
            return mLevel->IsPartOfTutorial();
    }
}

bool Tutorial::ShouldDisplayAnimationWindow() const {
    return mUserServices.GetSettingsService().GetControlType() != ControlType::Drag;
}

void Tutorial::InitDragAndDropTutorial() {
    CreateDragAndDropAnimation({-2.1f, -11.0f, UiLayer::root}, {-1.3f, -1.1f, UiLayer::root});
    CreateDragAndDropAnimation({0.9f, -11.0f, UiLayer::root}, {2.3f, -2.4f, UiLayer::root});
    CreateDragAndDropAnimation({0.9f, -11.0f, UiLayer::root}, {-2.0f, -1.1f, UiLayer::root});
}

void Tutorial::CreateDragAndDropAnimation(Pht::Vec3 handInitialPosition,
                                          Pht::Vec3 handDropPosition) {
    auto& scene = mScene.GetScene();
    auto& uiViewsContainer = mScene.GetUiViewsContainer();
    
    auto dragAndDropAnimation = std::make_unique<DragAndDropAnimation>(mEngine, 1.4f, true);
    dragAndDropAnimation->mContainer = &scene.CreateSceneObject(uiViewsContainer);
    dragAndDropAnimation->mHandAnimation.Init(*dragAndDropAnimation->mContainer);
    
    auto animationDuration = 3.8f;
    auto& animationSystem = mEngine.GetAnimationSystem();
    dragAndDropAnimation->mAnimation =
        &animationSystem.CreateAnimation(*dragAndDropAnimation->mContainer,
                                         {{.mTime = 0.0f}, {.mTime = animationDuration}});

    auto& handAnimation = dragAndDropAnimation->mHandAnimation;
    
    auto waitDuration = 0.3f;
    auto dragDuration = 2.3f;
    
    Pht::Vec3 handAdjustment {0.0f, 0.9f, 0.0f};
    if (mEngine.GetRenderer().GetBottomPaddingHeight() == 0.0f) {
        handInitialPosition += handAdjustment;
        handDropPosition += handAdjustment;
    }
    
    std::vector<Pht::Keyframe> handAnimationKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation, &handInitialPosition] () {
                handAnimation.StartInNotTouchingScreenState(handInitialPosition, 115.0f, 10.0f);
            }
        },
        {
            .mTime = waitDuration,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation, dragDuration] () {
                handAnimation.BeginTouch(dragDuration);
            }
        },
        {
            .mTime = waitDuration + 0.2f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = waitDuration + 0.2f + dragDuration,
            .mPosition = handDropPosition
        },
        {
            .mTime = waitDuration + 0.4f + dragDuration,
            .mPosition = handDropPosition
        },
        {
            .mTime = animationDuration - 0.1f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = animationDuration
        }
    };
    
    auto& handPhtAnimation =
        animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes);
    handPhtAnimation.SetInterpolation(Pht::Interpolation::Cosine);

    mDragAndDropAnimations.push_back(std::move(dragAndDropAnimation));
}

void Tutorial::StartDragAndDropAnimation(int index) {
    auto& dragAndDropAnimation = *mDragAndDropAnimations[index];
    dragAndDropAnimation.mAnimation->Play();
    dragAndDropAnimation.mContainer->SetIsVisible(true);
}

void Tutorial::StopDragAndDropAnimations() {
    for (auto& dragAndDropAnimation: mDragAndDropAnimations) {
        dragAndDropAnimation->mAnimation->Stop();
        dragAndDropAnimation->mContainer->SetIsVisible(false);
    }
}

void Tutorial::StartTapToRotateAnimation(Pht::Vec3 position) {
    Pht::Vec3 handAdjustment {0.0f, 0.9f, 0.0f};
    if (mEngine.GetRenderer().GetBottomPaddingHeight() == 0.0f) {
        position += handAdjustment;
    }
    
    mHandAnimation.Start(position, 115.0f);
}
