#ifndef GameController_hpp
#define GameController_hpp

// Engine includes.
#include "CameraShake.hpp"

// Game includes.
#include "Field.hpp"
#include "GameLogic.hpp"
#include "ScenePlayingField.hpp"
#include "GameScene.hpp"
#include "ScrollController.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "CollapsingFieldAnimationSystem.hpp"
#include "FlashingBlocksAnimationSystem.hpp"
#include "WeldsAnimationSystem.hpp"
#include "BlueprintSlotsFilledAnimation.hpp"
#include "EffectManager.hpp"
#include "PieceDropParticleEffect.hpp"
#include "PieceTrailParticleEffect.hpp"
#include "LevelResources.hpp"
#include "Level.hpp"
#include "PieceResources.hpp"
#include "GhostPieceBlocks.hpp"
#include "GameHudResources.hpp"
#include "GameViewControllers.hpp"
#include "StoreController.hpp"
#include "PreviewPiecesAnimation.hpp"
#include "PreviewPiecesRotationAnimation.hpp"
#include "ActivePreviewPieceAnimation.hpp"
#include "FewMovesAlertAnimation.hpp"
#include "AddingMovesAnimation.hpp"
#include "SlidingText.hpp"
#include "MediumText.hpp"
#include "ScoreTexts.hpp"
#include "BlastArea.hpp"
#include "Shield.hpp"
#include "FieldBottomGlow.hpp"
#include "ValidAreaAnimation.hpp"
#include "BombsAnimation.hpp"
#include "AsteroidAnimation.hpp"
#include "FallingPieceScaleAnimation.hpp"
#include "LevelCompletedController.hpp"
#include "Tutorial.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class FallingPieceAnimation;
    class DraggedPieceAnimation;
    
    class GameController {
    public:
        enum class Command {
            RestartLevel,
            GoToMap,
            GoToNextLevel,
            None
        };

        GameController(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       UserServices& userServices);
    
        void Init(int levelId);
        Command Update();
        
        const LevelResources& GetLevelResources() const {
            return mLevelResources;
        }
        
        PieceResources& GetPieceResources() {
            return mPieceResources;
        }
    
    private:
        Command UpdateGame();
        Command UpdateInPausedState();
        void UpdateGameMenu();
        void UpdateHowToPlayDialog();
        void UpdateLevelInfoDialog();
        void UpdateSettingsMenu();
        Command UpdateInPausedStateNoLivesDialog();
        Command UpdateRestartConfirmationDialog();
        Command UpdateMapConfirmationDialog();
        Command UpdateInPausedStateStore();
        Command UpdateSubState();
        void UpdateInLevelIntroState();
        void UpdateTutorialDialogs();
        void StartLevelObjectiveAnimation();
        Command UpdateInLevelCompletedState();
        Command UpdateInOutOfMovesState();
        void UpdateInOutOfMovesStateOutOfMovesAnimation();
        Command UpdateOutOfMovesDialog();
        int CalculateProgressInLevelForAnalytics();
        void UpdateInOutOfMovesStateStore();
        Command UpdateInGameOverState();
        Command UpdateGameOverDialog();
        Command UpdateInGameOverStateNoLivesDialog();
        Command UpdateInGameOverStateStore();
        void AddMovesAndGoToPlayingState();
        void RefillLives();
        bool ShouldUndoMove(bool isInBetweenMoves);
        void ChangeGameState(GameLogic::Result gameLogicResult);
        void GoToPlayingState();
        void GoToPausedStateNoLivesDialog();
        void GoToPausedStateRestartConfirmationDialog();
        void GoToPausedStateMapConfirmationDialog();
        void GoToPausedStateStore();
        void GoToPausedStateSettingsMenu();
        void GoToPausedStateLevelInfoDialog();
        void GoToPausedStateHowToPlayDialog();
        void GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade,
                                     SlidingMenuAnimation::SlideDirection slideDirection);
        void GoToOutOfMovesStateOutOfMovesAnimation();
        void GoToOutOfMovesStateOutOfMovesDialog(SlidingMenuAnimation::SlideDirection slideDirection,
                                                 SlidingMenuAnimation::UpdateFade updateFade);
        void GoToOutOfMovesStateStore();
        void GoToGameOverStateGameOverDialog();
        void GoToGameOverStateNoLivesDialog();
        void GoToGameOverStateStore();

        enum class GameState {
            LevelIntro,
            TutorialDialog,
            Playing,
            Paused,
            LevelCompleted,
            OutOfMoves,
            GameOver
        };
        
        enum class LevelIntroState {
            Overview,
            ObjectiveAnimation
        };
        
        enum class PausedState {
            GameMenu,
            HowToPlayDialog,
            LevelInfoDialog,
            SettingsMenu,
            NoLivesDialog,
            RestartConfirmationDialog,
            MapConfirmationDialog,
            Store
        };
        
        enum class OutOfMovesState {
            OutOfMovesAnimation,
            OutOfMovesDialog,
            Store
        };
        
        enum class GameOverState {
            GameOverDialog,
            NoLivesDialog,
            Store
        };
                                    
        GameState mState {GameState::LevelIntro};
        LevelIntroState mLevelIntroState {LevelIntroState::Overview};
        PausedState mPausedState {PausedState::GameMenu};
        OutOfMovesState mOutOfMovesState {OutOfMovesState::OutOfMovesDialog};
        GameOverState mGameOverState {GameOverState::GameOverDialog};
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        PieceResources mPieceResources;
        GhostPieceBlocks mGhostPieceBlocks;
        LevelResources mLevelResources;
        GameHudResources mGameHudResources;
        GameViewControllers mGameViewControllers;
        Field mField;
        CollapsingFieldAnimationSystem mCollapsingFieldAnimation;
        FlashingBlocksAnimationSystem mFlashingBlocksAnimation;
        WeldsAnimationSystem mWeldsAnimationSystem;
        ScrollController mScrollController;
        ValidAreaAnimation mValidAreaAnimation;
        Pht::CameraShake mCameraShake;
        GameScene mScene;
        StoreController mStoreController;
        EffectManager mEffectManager;
        PieceDropParticleEffect mPieceDropParticleEffect;
        PieceTrailParticleEffect mPieceTrailParticleEffect;
        BlastArea mBlastArea;
        Shield mShield;
        FieldBottomGlow mFieldBottomGlow;
        SlidingText mSlidingText;
        ScoreTexts mScoreTexts;
        MediumText mMediumText;
        FallingPieceScaleAnimation mFallingPieceScaleAnimation;
        Tutorial mTutorial;
        GameLogic mGameLogic;
        FallingPieceAnimation& mFallingPieceAnimation;
        DraggedPieceAnimation& mDraggedPieceAnimation;
        ActivePreviewPieceAnimation mActivePreviewPieceAnimation;
        PreviewPiecesAnimation mPreviewPiecesAnimation;
        PreviewPiecesRotationAnimation mPreviewPiecesRotationAnimation;
        FewMovesAlertAnimation mFewMovesAlertAnimation;
        AddingMovesAnimation mAddingMovesAnimation;
        BlueprintSlotsFilledAnimation mBlueprintSlotsFilledAnimation;
        BombsAnimation mBombsAnimation;
        AsteroidAnimation mAsteroidAnimation;
        FlyingBlocksAnimation mFlyingBlocksAnimation;
        ScenePlayingField mScenePlayingField;
        LevelCompletedController mLevelCompletedController;
        std::unique_ptr<Level> mLevel;
        bool mIsInBetweenMoves {false};
        bool mUndoMovePending {false};
    };
}

#endif
