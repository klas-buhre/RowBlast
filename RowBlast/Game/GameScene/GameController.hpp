#ifndef GameController_hpp
#define GameController_hpp

// Engine includes.
#include "CameraShake.hpp"

// Game includes.
#include "Field.hpp"
#include "GameLogic.hpp"
#include "GameSceneRenderer.hpp"
#include "GameScene.hpp"
#include "ScrollController.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "CollapsingFieldAnimation.hpp"
#include "FlashingBlocksAnimation.hpp"
#include "WeldsAnimation.hpp"
#include "BlueprintSlotsFilledAnimation.hpp"
#include "EffectManager.hpp"
#include "PieceDropParticleEffect.hpp"
#include "LevelResources.hpp"
#include "Level.hpp"
#include "PieceResources.hpp"
#include "GameViewControllers.hpp"
#include "PreviewPiecesAnimation.hpp"
#include "ClearLastBlocksAnimation.hpp"
#include "SlidingTextAnimation.hpp"
#include "BlastRadiusAnimation.hpp"
#include "BombsAnimation.hpp"
#include "LevelCompletedController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
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
                       UserData& userData,
                       Settings& settings);
    
        void StartLevel(int levelIndex);
        Command Update();
        
        const LevelResources& GetLevelResources() const {
            return mLevelResources;
        }
    
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
        Command UpdateInLevelCompletedState();
        Command UpdateNoMovesDialog();
        Command UpdateInGameOverState();
        Command UpdateGameOverDialog();
        void ChangeGameState(GameLogic::Result gameLogicResult);
        void GoToPlayingState();
        void GoToPausedStateNoLivesDialog();
        void GoToPausedStateRestartConfirmationDialog();
        void GoToPausedStateMapConfirmationDialog();
        void GoToPausedStateSettingsMenu();
        void GoToPausedStateGameMenu(SlidingMenuAnimation::UpdateFade updateFade);
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
        
        enum class GameOverState {
            GameOverDialog,
            NoLivesDialog
        };
                                    
        GameState mState {GameState::LevelIntro};
        LevelIntroState mLevelIntroState {LevelIntroState::Overview};
        PausedState mPausedState {PausedState::GameMenu};
        GameOverState mGameOverState {GameOverState::GameOverDialog};
        Pht::IEngine& mEngine;
        UserData& mUserData;
        GameViewControllers mGameViewControllers;
        Field mField;
        CollapsingFieldAnimation mCollapsingFieldAnimation;
        FlashingBlocksAnimation mFlashingBlocksAnimation;
        WeldsAnimation mWeldsAnimation;
        ScrollController mScrollController;
        Pht::CameraShake mCameraShake;
        GameScene mScene;
        EffectManager mEffectManager;
        PieceDropParticleEffect mPieceDropParticleEffect;
        BlastRadiusAnimation mBlastRadiusAnimation;
        SlidingTextAnimation mSlidingTextAnimation;
        ClearLastBlocksAnimation mClearLastBlocksAnimation;
        GameLogic mGameLogic;
        FallingPieceAnimation& mFallingPieceAnimation;
        PieceResources mPieceResources;
        LevelResources mLevelResources;
        PreviewPiecesAnimation mPreviewPiecesAnimation;
        BlueprintSlotsFilledAnimation mBlueprintSlotsFilledAnimation;
        BombsAnimation mBombsAnimation;
        FlyingBlocksAnimation mFlyingBlocksAnimation;
        GameSceneRenderer mRenderer;
        LevelCompletedController mLevelCompletedController;
        std::unique_ptr<Level> mLevel;
        bool mShouldUpdateGameLogic {false};
    };
}

#endif
