#ifndef PreviewPiecesAnimation_hpp
#define PreviewPiecesAnimation_hpp

// Game includes.
#include "PreviewPieceGroupAnimation.hpp"

namespace BlocksGame {
    class GameScene;
    class GameLogic;
    
    class PreviewPiecesAnimation {
    public:
        PreviewPiecesAnimation(GameScene& scene, GameLogic& gameLogic);
        
        void Init();
        void Update(float dt);
        
    private:
        void StartNextPieceAndSwitchingAnimation();
        void StartSwitchingPiecesAnimation();
        float UpdateTime(float dt);
        void GoToSwitchingPieceState();
        void GoToNextPieceAndSwitchState();
        
        enum class State {
            SwitchingPiece,
            NextPieceAndSwitch,
            Inactive
        };
        
        GameScene& mScene;
        GameLogic& mGameLogic;
        State mState {State::Inactive};
        PreviewPieceGroupAnimation mSwitchPieceAnimation;
        PreviewPieceGroupAnimation mNextPieceAnimation;
        float mElapsedTime {0.0f};
    };
}

#endif
