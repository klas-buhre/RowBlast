#ifndef LevelCompletedController_hpp
#define LevelCompletedController_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "LevelCompletedDialogController.hpp"
#include "SlidingFieldAnimation.hpp"
#include "FireworksParticleEffect.hpp"
#include "ConfettiParticleEffect.hpp"
#include "StarsAnimation.hpp"

namespace Pht {
    class IEngine;
    class CameraShake;
}

namespace RowBlast {
    class GameScene;
    class GameViewControllers;
    class SlidingTextAnimation;
    class ClearLastBlocksAnimation;
    class GameLogic;
    class UserData;
    class Level;
    class CommonResources;

    class LevelCompletedController {
    public:
        LevelCompletedController(Pht::IEngine& engine,
                                 GameScene& gameScene,
                                 GameViewControllers& gameViewControllers,
                                 SlidingTextAnimation& slidingTextAnimation,
                                 ClearLastBlocksAnimation& clearLastBlocksAnimation,
                                 GameLogic& gameLogic,
                                 UserData& userData,
                                 const CommonResources& commonResources,
                                 Pht::CameraShake& cameraShake);

        void Init(const Level& level);
        void Start();
        LevelCompletedDialogController::Result Update();
        
    private:
        void StartLevelCompletedTextAnimation();
        void UpdateInObjectiveAchievedAnimationState();
        void UpdateInClearingLastBlocksState();
        void UpdateInSlidingOutFieldAnimationState();
        void UpdateFireworksAndConfetti();
        void UpdateInStarsAppearingAnimationState();
        LevelCompletedDialogController::Result UpdateLevelCompletedDialog();
        
        enum class State {
            ObjectiveAchievedAnimation,
            ClearingLastBlocks,
            SlidingOutFieldAnimation,
            FireworksAndConfetti,
            StarsAppearingAnimation,
            LevelCompletedDialog
        };
        
        State mState {State::ObjectiveAchievedAnimation};
        Pht::IEngine& mEngine;
        GameScene& mGameScene;
        GameViewControllers& mGameViewControllers;
        SlidingTextAnimation& mSlidingTextAnimation;
        ClearLastBlocksAnimation& mClearLastBlocksAnimation;
        GameLogic& mGameLogic;
        UserData& mUserData;
        const Level* mLevel {nullptr};
        Pht::FadeEffect mFadeEffect;
        SlidingFieldAnimation mSlidingFieldAnimation;
        FireworksParticleEffect mFireworksParticleEffect;
        ConfettiParticleEffect mConfettiParticleEffect;
        StarsAnimation mStarsAnimation;
        float mTimeSpentInFireworksAndConfettiState {0.0f};
    };
}

#endif
