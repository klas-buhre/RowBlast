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
#include "ClearLastBlocksAnimation.hpp"

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
    class FlyingBlocksAnimation;
    class Field;

    class LevelCompletedController {
    public:
        LevelCompletedController(Pht::IEngine& engine,
                                 GameScene& gameScene,
                                 GameViewControllers& gameViewControllers,
                                 SlidingTextAnimation& slidingTextAnimation,
                                 GameLogic& gameLogic,
                                 UserData& userData,
                                 const CommonResources& commonResources,
                                 Pht::CameraShake& cameraShake,
                                 Field& field,
                                 FlyingBlocksAnimation& flyingBlocksAnimation);

        void Init(const Level& level);
        void Start();
        LevelCompletedDialogController::Result Update();
        
    private:
        void StartLevelCompletedTextAnimation();
        void UpdateInObjectiveAchievedAnimationState();
        void UpdateObjectiveAchievedAnimation();
        void UpdateInConfettiState();
        void UpdateInClearingLastBlocksState();
        void UpdateInSlidingOutFieldAnimationState();
        void UpdateInFireworksState();
        void UpdateFireworksAndConfetti();
        void UpdateInStarsAppearingAnimationState();
        LevelCompletedDialogController::Result UpdateLevelCompletedDialog();
        
        enum class State {
            ObjectiveAchievedAnimation,
            Confetti,
            ClearingLastBlocks,
            SlidingOutFieldAnimation,
            Fireworks,
            StarsAppearingAnimation,
            LevelCompletedDialog
        };
        
        State mState {State::ObjectiveAchievedAnimation};
        Pht::IEngine& mEngine;
        GameScene& mGameScene;
        GameViewControllers& mGameViewControllers;
        SlidingTextAnimation& mSlidingTextAnimation;
        GameLogic& mGameLogic;
        UserData& mUserData;
        const Level* mLevel {nullptr};
        Pht::FadeEffect mFadeEffect;
        ClearLastBlocksAnimation mClearLastBlocksAnimation;
        SlidingFieldAnimation mSlidingFieldAnimation;
        FireworksParticleEffect mFireworksParticleEffect;
        ConfettiParticleEffect mConfettiParticleEffect;
        StarsAnimation mStarsAnimation;
        float mElapsedTime {0.0f};
    };
}

#endif
