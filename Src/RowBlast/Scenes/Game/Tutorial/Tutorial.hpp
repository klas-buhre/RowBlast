#ifndef Tutorial_hpp
#define Tutorial_hpp

// Engine includes.
#include "FadeEffect.hpp"
#include "GuiViewManager.hpp"

// Game includes.
#include "HandAnimation.hpp"
#include "PlacePieceWindowController.hpp"
#include "FillRowsWindowController.hpp"
#include "SwitchPieceWindowController.hpp"
#include "OtherMovesWindowController.hpp"
#include "CascadingDialogController.hpp"
#include "SameColorDialogController.hpp"
#include "LaserDialogController.hpp"
#include "BombDialogController.hpp"
#include "LevelBombDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class Level;
    class Piece;
    class Move;
    
    class Tutorial {
    public:
        enum class Result {
            Play,
            TutorialHasFocus
        };
        
        Tutorial(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources);
        
        void Init(const Level& level);
        void Update();
        Result UpdateDialogs();
        Result OnLevelStart();
        void OnPause();
        void OnResumePlaying();
        void OnNewMove(int numMovesUsedIncludingCurrent);
        void OnSelectMove(int numMovesUsedIncludingCurrent);
        void OnSwitchPiece(int numMovesUsedIncludingCurrent, const Piece& pieceType);
        void OnChangeVisibleMoves(int numMovesUsedIncludingCurrent, const Move& firstMove);
        bool IsSwitchPieceAllowed(int numMovesUsedIncludingCurrent) const;
        bool IsMoveAllowed(int numMovesUsedIncludingCurrent,
                           const Piece& pieceType,
                           const Move& move) const;
        bool IsGestureControlsAllowed() const;
        bool IsUndoMoveAllowed(int numMovesUsedIncludingCurrent) const;

    private:
        enum class Controller {
            PlacePieceWindow,
            FillRowsWindow,
            SwitchPieceWindow,
            OtherMovesWindow,
            CascadingDialog,
            SameColorDialog,
            LaserDialog,
            BombDialog,
            LevelBombDialog,
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
        PlacePieceWindowController mPlacePieceWindowController;
        FillRowsWindowController mFillRowsWindowController;
        SwitchPieceWindowController mSwitchPieceWindowController;
        OtherMovesWindowController mOtherMovesWindowController;
        CascadingDialogController mCascadingDialogController;
        SameColorDialogController mSameColorDialogController;
        LaserDialogController mLaserDialogController;
        BombDialogController mBombDialogController;
        LevelBombDialogController mLevelBombDialogController;
    };
}

#endif
