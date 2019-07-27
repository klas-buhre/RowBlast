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
    class BlastRadiusAnimation;
    
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
                 const BlastRadiusAnimation& blastRadiusAnimation,
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
        bool IsGestureControlsAllowed() const;
        bool IsUndoMoveAllowed(int numMovesUsedIncludingCurrent) const;

    private:
        enum class Controller {
            PlacePieceWindow,
            FillRowsWindow,
            SwitchPieceWindow,
            SwitchPieceHintWindow,
            SwitchPiece2Window,
            SwitchPiece3Window,
            OtherMovesWindow,
            OtherMoves2Window,
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
        
        Pht::IEngine& mEngine;
        GameScene& mScene;
        const Level* mLevel {nullptr};
        Controller mActiveViewController {Controller::None};
        Pht::FadeEffect mFadeEffect;
        HandAnimation mHandAnimation;
        Pht::GuiViewManager mViewManager;
        TutorialWindowController mPlacePieceWindowController;
        TutorialWindowController mFillRowsWindowController;
        TutorialWindowController mSwitchPieceWindowController;
        TutorialWindowController mSwitchPieceHintWindowController;
        TutorialWindowController mSwitchPiece2WindowController;
        TutorialWindowController mSwitchPiece3WindowController;
        TutorialWindowController mOtherMovesWindowController;
        TutorialWindowController mOtherMoves2WindowController;
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
