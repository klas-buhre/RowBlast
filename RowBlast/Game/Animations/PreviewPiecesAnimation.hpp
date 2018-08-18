#ifndef PreviewPiecesAnimation_hpp
#define PreviewPiecesAnimation_hpp

// Game includes.
#include "PreviewPieceGroupAnimation.hpp"

namespace RowBlast {
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
        void StartRemoveActivePieceAnimation();
        float UpdateTime(float dt);
        void GoToSwitchingPieceState();
        void GoToNextPieceAndSwitchState();
        void GoToRemovingActivePieceState();
        
        enum class State {
            SwitchingPiece,
            NextPieceAndSwitch,
            RemovingActivePiece,
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
