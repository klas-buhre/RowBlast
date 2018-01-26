#ifndef GameController_hpp
#define GameController_hpp

// Game includes.
#include "Field.hpp"
#include "GameLogic.hpp"
#include "GameSceneRenderer.hpp"
#include "GameScene.hpp"
#include "ScrollController.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "CollapsingFieldAnimation.hpp"
#include "FlashingBlocksAnimation.hpp"
#include "BlueprintSlotsFilledAnimation.hpp"
#include "ExplosionParticleEffect.hpp"
#include "RowExplosionParticleEffect.hpp"
#include "GameHud.hpp"
#include "PieceDropParticleEffect.hpp"
#include "LevelResources.hpp"
#include "Level.hpp"
#include "PieceResources.hpp"
#include "GameViewControllers.hpp"
#include "ClearLastBlocksAnimation.hpp"
#include "SlidingTextAnimation.hpp"
#include "BlastRadiusAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class CommonViewControllers;
    class UserData;
    class Settings;
    class FallingPieceAnimation;
    
    class GameController {
    public:
        enum class Command {
            RestartGame,
            GoToMap,
            GoToNextLevel,
            None
        };

        GameController(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       CommonViewControllers& commonViewControllers,
                       UserData& userData,
                       Settings& settings);
    
        void StartLevel(int levelIndex);
        Command Update();
        void RenderScene();
    
    private:
        Command UpdateGame();
        Command UpdateInPausedState();
        void UpdateGameMenu();
        void UpdateSettingsMenu();
        Command UpdateNoLivesDialog();
        Command UpdateRestartConfirmationDialog();
        Command UpdateMapConfirmationDialog();
        Command UpdateSubState();
        void UpdateInLevelIntroState();
        void StartLevelObjectiveAnimation();
        Command UpdateNoMovesDialog();
        Command UpdateInGameOverState();
        Command UpdateGameOverDialog();
        Command UpdateInLevelCompletedState();
        Command UpdateLevelCompletedDialog();
        void ChangeGameState(GameLogic::Result gameLogicResult);
        void StartLevelCompletedAnimation();
        void GoToPlayingState();
        void GoToPausedStateNoLivesDialog();
        void GoToPausedStateRestartConfirmationDialog();
        void GoToPausedStateMapConfirmationDialog();
        void GoToPausedStateSettingsMenu();
        void GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade);
        void GoToLevelCompletedStateLevelCompletedDialog();
        void GoToGameOverStateGameOverDialog();
        void GoToGameOverStateNoLivesDialog();
        
        enum class GameState {
            LevelIntro,
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
            SettingsMenu,
            NoLivesDialog,
            RestartConfirmationDialog,
            MapConfirmationDialog
        };
        
        enum class LevelCompletedState {
            ObjectiveAchievedAnimation,
            ClearingLastBlocks,
            LevelCompletedDialog
        };
        
        enum class GameOverState {
            GameOverDialog,
            NoLivesDialog
        };
                                    
        GameState mState {GameState::LevelIntro};
        LevelIntroState mLevelIntroState {LevelIntroState::Overview};
        PausedState mPausedState {PausedState::GameMenu};
        LevelCompletedState mLevelCompletedState {LevelCompletedState::ObjectiveAchievedAnimation};
        GameOverState mGameOverState {GameOverState::GameOverDialog};
        Pht::IEngine& mEngine;
        UserData& mUserData;
        GameViewControllers mGameViewControllers;
        Field mField;
        CollapsingFieldAnimation mCollapsingFieldAnimation;
        FlashingBlocksAnimation mFlashingBlocksAnimation;
        ScrollController mScrollController;
        GameScene mScene;
        ExplosionParticleEffect mExplosionParticleEffect;
        RowExplosionParticleEffect mRowExplosionParticleEffect;
        PieceDropParticleEffect mPieceDropParticleEffect;
        BlastRadiusAnimation mBlastRadiusAnimation;
        SlidingTextAnimation mSlidingTextAnimation;
        ClearLastBlocksAnimation mClearLastBlocksAnimation;
        GameLogic mGameLogic;
        FallingPieceAnimation& mFallingPieceAnimation;
        PieceResources mPieceResources;
        LevelResources mLevelResources;
        FlyingBlocksAnimation mFlyingBlocksAnimation;
        BlueprintSlotsFilledAnimation mBlueprintSlotsFilledAnimation;
        GameHud mHud;
        GameSceneRenderer mRenderer;
        std::unique_ptr<Level> mLevel;
        bool mShouldUpdateGameLogic {false};
    };
}

#endif
