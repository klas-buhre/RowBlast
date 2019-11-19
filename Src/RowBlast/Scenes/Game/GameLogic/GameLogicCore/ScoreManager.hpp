#ifndef ScoreManager_hpp
#define ScoreManager_hpp

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class IGameLogic;
    class SmallText;
    class EffectManager;

    class ScoreManager {
    public:
        ScoreManager(Pht::IEngine& engine,
                     IGameLogic& gameLogic,
                     SmallText& smallTextAnimation,
                     EffectManager& effectManager);

        void Init();
        void OnSpawnPiece();
        void OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells);
        void OnClearedNoFilledRows();
        void OnFilledSlots(int numSlots);
        void OnUndoMove();
        void GoToCascadingState();
        
        static int CalculateExtraPoints(int currentScore,
                                        int numMovesLeft,
                                        int numMovesAtLevelStart);
        
    private:
        void OnClearedFilledRowsInPieceSpawnedState(int numClearedRows);
        void OnClearedFilledRowsInCascadingState(int numClearedRows);
        void OnClearedFiveRows();
        void OnClearedFourRows();
        void DetectCascade();
        
        enum class State {
            PieceSpawned,
            Cascading,
            Inactive
        };

        Pht::IEngine& mEngine;
        IGameLogic& mGameLogic;
        SmallText& mSmallText;
        EffectManager& mEffectManager;
        State mState {State::Inactive};
        int mNumCombos {0};
        int mNumCascades {0};
    };
}

#endif
