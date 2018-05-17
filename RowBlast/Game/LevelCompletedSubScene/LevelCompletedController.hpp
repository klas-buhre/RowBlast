#ifndef LevelCompletedController_hpp
#define LevelCompletedController_hpp

#include "LevelCompletedDialogController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameViewControllers;
    class SlidingTextAnimation;
    class ClearLastBlocksAnimation;
    class GameLogic;
    class UserData;
    class Level;

    class LevelCompletedController {
    public:
        LevelCompletedController(Pht::IEngine& engine,
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
    };
}

#endif
