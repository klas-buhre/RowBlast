#ifndef LevelCompletedController_hpp
#define LevelCompletedController_hpp

// Game includes.
#include "LevelCompletedDialogController.hpp"
#include "SlidingFieldAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class GameViewControllers;
    class SlidingTextAnimation;
    class ClearLastBlocksAnimation;
    class GameLogic;
    class UserData;
    class Level;

    class LevelCompletedController {
    public:
        LevelCompletedController(Pht::IEngine& engine,
                                 GameScene& gameScene,
                                 GameViewControllers& gameViewControllers,
                                 SlidingTextAnimation& slidingTextAnimation,
                                 ClearLastBlocksAnimation& clearLastBlocksAnimation,
                                 GameLogic& gameLogic,
                                 UserData& userData);

        void Init(const Level& level);
        void Start();
        LevelCompletedDialogController::Result Update();
        
    private:
        void StartLevelCompletedTextAnimation();
        void GoToLevelCompletedDialogState();
        LevelCompletedDialogController::Result UpdateLevelCompletedDialog();
        
        enum class State {
            ObjectiveAchievedAnimation,
            ClearingLastBlocks,
            SlidingOutFieldAnimation,
            LevelCompletedDialog
        };
        
        State mState {State::ObjectiveAchievedAnimation};
        Pht::IEngine& mEngine;
        GameViewControllers& mGameViewControllers;
        SlidingTextAnimation& mSlidingTextAnimation;
        ClearLastBlocksAnimation& mClearLastBlocksAnimation;
        GameLogic& mGameLogic;
        UserData& mUserData;
        const Level* mLevel {nullptr};
        SlidingFieldAnimation mSlidingFieldAnimation;
    };
}

#endif
