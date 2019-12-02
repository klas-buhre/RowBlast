#ifndef FinalScoreAnimation_hpp
#define FinalScoreAnimation_hpp

// Game includes.
#include "ScoreTexts.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class GameLogic;
    class CommonResources;

    class FinalScoreAnimation {
    public:
        FinalScoreAnimation(Pht::IEngine& engine,
                            GameLogic& gameLogic,
                            GameScene& scene,
                            const CommonResources& commonResources);
        
        void Init();
        void Start();
        void Update();
        
    private:
        void UpdateInMovesCountdownState();
        void UpdateInMovesCountdownAndScoreTextState();
        void UpdateMovesCountdown();
        void UpdateScoreText();
        
        enum class State {
            Inactive,
            MovesCountdown,
            MovesCountdownAndScoreText,
            Done
        };

        Pht::IEngine& mEngine;
        GameLogic& mGameLogic;
        GameScene& mScene;
        ScoreTexts mScoreTexts;
        State mState {State::Inactive};
        int mBonusPoints {0};
        int mScoreAtLevelCompleted {0};
        float mElapsedTime {0.0f};
    };
}
#endif
