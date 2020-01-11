#ifndef Tutorial_hpp
#define Tutorial_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "HandAnimation.hpp"
#include "TutorialWindowController.hpp"
#include "PlayOnYourOwnWindowController.hpp"
#include "CascadingDialogController.hpp"
#include "SameColorDialogController.hpp"
#include "LaserDialogController.hpp"
#include "BombDialogController.hpp"
#include "LevelBombDialogController.hpp"
#include "AsteroidDialogController.hpp"
#include "ClickInputHandler.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class Level;
    class Piece;
    class Move;
    class UserServices;
    class BlastArea;
    
    class Tutorial {
    public:
        enum class Result {
            Play,
            TutorialHasFocus
        };
        
        Tutorial(Pht::IEngine& engine,
                 GameScene& scene,
                 const CommonResources& commonResources,
                 const PieceResources& pieceResources,
                 const GhostPieceBlocks& ghostPieceBlocks,
                 const LevelResources& levelResources,
                 const BlastArea& blastRadiusAnimation,
                 const UserServices& userServices);
        
        void Init(const Level& level);
        void Update();
        Result UpdateDialogs();
        Result OnLevelStart();
        void OnPause();
        void OnResumePlaying();
        void OnNewMove(int numMovesUsedIncludingCurrent);
        void OnSelectMove(int numMovesUsedIncludingCurrent);
        void OnSwitchPiece(int numMovesUsedIncludingCurrent, const Piece& pieceType);
        void OnChangeVisibleMoves(int numMovesUsedIncludingCurrent,
                                  const ClickInputHandler::VisibleMoves& visibleMoves);
        bool IsSwitchPieceAllowed() const;
        bool IsSeeMoreMovesAllowed(int numMovesUsedIncludingCurrent) const;
        bool IsMoveAllowed(int numMovesUsedIncludingCurrent,
                           const Piece& pieceType,
                           const Move& move);
        bool IsControlTypeChangeAllowed() const;
        bool IsUndoMoveAllowed(int numMovesUsedIncludingCurrent) const;

    private:
        enum class Controller {
            PlacePieceWindow,
            PlacePieceHintWindow,
            FillRowsWindow,
            FillRowsHintWindow,
            SwitchPieceWindow,
            SwitchPieceHintWindow,
            PlaceIPieceWindow,
            SwitchPiece2Window,
            SwitchPiece3Window,
            PlaceBPieceWindow,
            OtherMovesWindow,
            PlaceMirroredSevenPieceWindow,
            OtherMoves2Window,
            PlaceYellowPieceHintWindow,
            PlayOnYourOwnWindow,
            CascadingDialog,
            SameColorDialog,
            LaserDialog,
            BombDialog,
            LevelBombDialog,
            AsteroidDialog,
            None
        };

        void OnNewMoveFirstLevel(int numMovesUsedIncludingCurrent);
        void OnNewMoveSecondLevel(int numMovesUsedIncludingCurrent);
        void SetActiveViewController(Controller controller);
        void SendAnayticsEvent(const std::string& id);
        bool IsLevelPartOfTutorial() const;
        
        Pht::IEngine& mEngine;
        GameScene& mScene;
        const UserServices& mUserServices;
        const Level* mLevel {nullptr};
        Controller mActiveViewController {Controller::None};
        Pht::FadeEffect mFadeEffect;
        HandAnimation mHandAnimation;
        Pht::GuiViewManager mViewManager;
        TutorialWindowController mPlacePieceWindowController;
        TutorialWindowController mPlacePieceHintWindowController;
        TutorialWindowController mFillRowsWindowController;
        TutorialWindowController mFillRowsHintWindowController;
        TutorialWindowController mSwitchPieceWindowController;
        TutorialWindowController mSwitchPieceHintWindowController;
        TutorialWindowController mPlaceIPieceWindowController;
        TutorialWindowController mSwitchPiece2WindowController;
        TutorialWindowController mSwitchPiece3WindowController;
        TutorialWindowController mPlaceBPieceWindowController;
        TutorialWindowController mOtherMovesWindowController;
        TutorialWindowController mPlaceMirroredSevenPieceWindowController;
        TutorialWindowController mOtherMoves2WindowController;
        TutorialWindowController mPlaceYellowPieceHintWindowController;
        PlayOnYourOwnWindowController mPlayOnYourOwnWindowController;
        CascadingDialogController mCascadingDialogController;
        SameColorDialogController mSameColorDialogController;
        LaserDialogController mLaserDialogController;
        BombDialogController mBombDialogController;
        LevelBombDialogController mLevelBombDialogController;
        AsteroidDialogController mAsteroidDialogController;
    };
}

#endif
