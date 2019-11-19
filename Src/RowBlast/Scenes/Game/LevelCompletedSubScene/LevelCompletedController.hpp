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
#include "Ufo.hpp"
#include "UfoAnimation.hpp"

namespace Pht {
    class IEngine;
    class CameraShake;
}

namespace RowBlast {
    class GameScene;
    class GameViewControllers;
    class SlidingText;
    class SmallText;
    class ClearLastBlocksAnimation;
    class GameLogic;
    class UserServices;
    class Level;
    class CommonResources;
    class FlyingBlocksAnimation;
    class Field;

    class LevelCompletedController {
    public:
        LevelCompletedController(Pht::IEngine& engine,
                                 GameScene& gameScene,
                                 GameViewControllers& gameViewControllers,
                                 SlidingText& slidingTextAnimation,
                                 SmallText& smallTextAnimation,
                                 GameLogic& gameLogic,
                                 UserServices& userServices,
                                 const CommonResources& commonResources,
                                 Pht::CameraShake& cameraShake,
                                 Field& field,
                                 FlyingBlocksAnimation& flyingBlocksAnimation);

        void Init(const Level& level);
        void Start();
        LevelCompletedDialogController::Result Update();
        
    private:
        void GoToObjectiveAchievedAnimationState();
        void StartLevelCompletedTextAnimation();
        void UpdateInWaitingState();
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
            Waiting,
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
        SlidingText& mSlidingText;
        SmallText& mSmallText;
        GameLogic& mGameLogic;
        UserServices& mUserServices;
        const Level* mLevel {nullptr};
        Pht::FadeEffect mFadeEffect;
        ClearLastBlocksAnimation mClearLastBlocksAnimation;
        SlidingFieldAnimation mSlidingFieldAnimation;
        FireworksParticleEffect mFireworksParticleEffect;
        ConfettiParticleEffect mConfettiParticleEffect;
        StarsAnimation mStarsAnimation;
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
        float mElapsedTime {0.0f};
        float mWaitTime {0.0f};
        int mNumStars {0};
    };
}

#endif
