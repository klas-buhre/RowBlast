#ifndef PreviewPiecesAnimation_hpp
#define PreviewPiecesAnimation_hpp

// Game includes.
#include "PreviewPieceGroupAnimation.hpp"

namespace RowBlast {
    class GameScene;
    class GameLogic;
    class ActivePreviewPieceAnimation;
    
    class PreviewPiecesAnimation {
    public:
        PreviewPiecesAnimation(GameScene& scene,
                               GameLogic& gameLogic,
                               ActivePreviewPieceAnimation& activePreviewPieceAnimation);
        
        void Init();
        void Update(float dt);
        
    private:
        void StartNextPieceAndSwitchingAnimation();
        void StartNextPieceAndRefillActiveAnimation();
        void StartNextPieceAnimation(int destinationSlotInSelectables);
        void StartNextPieceAndRefillSelectable0Animation();
        void StartNextPieceAndRefillSelectable1Animation();
        void StartSwitchingPiecesAnimation();
        void StartRemoveActivePieceAnimation();
        float UpdateTime(float dt);
        void GoToSwitchingPieceState();
        void GoToNextPieceAndSwitchState();
        void GoToNextPieceState();
        void GoToRemovingActivePieceState();
        void OnNextPieceAnimationFinished();
        
        enum class State {
            SwitchingPiece,
            NextPieceAndSwitch,
            NextPiece,
            RemovingActivePiece,
            Inactive
        };
        
        GameScene& mScene;
        GameLogic& mGameLogic;
        ActivePreviewPieceAnimation& mActivePreviewPieceAnimation;
        State mState {State::Inactive};
        PreviewPieceGroupAnimation mSwitchPieceAnimation;
        PreviewPieceGroupAnimation mNextPieceAnimation;
        float mElapsedTime {0.0f};
    };
}

#endif
