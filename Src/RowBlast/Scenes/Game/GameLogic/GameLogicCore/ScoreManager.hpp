#ifndef ScoreManager_hpp
#define ScoreManager_hpp

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class IGameLogic;
    class MediumText;
    class EffectManager;

    class ScoreManager {
    public:
        ScoreManager(Pht::IEngine& engine,
                     IGameLogic& gameLogic,
                     MediumText& mediumTextAnimation,
                     EffectManager& effectManager);

        void Init();
        void OnSpawnPiece();
        void OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells,
                                 Pht::Optional<int> landedPieceId = Pht::Optional<int>{});
        void OnClearedNoFilledRows();
        void OnBombExplosionFinished(float numBlocksCleared, const Pht::IVec2& gridPosition);
        void OnLaserFinished(float numBlocksCleared, const Pht::IVec2& gridPosition);
        void OnFilledSlots(const Field::PieceFilledSlots& slotsCoveredByPiece);
        void OnUndoMove();
        void GoToCascadingState();
        
        static int CalculateExtraPoints(int currentScore,
                                        int numMovesLeft,
                                        int numMovesAtLevelStart);
        
    private:
        void OnClearedFilledRowsInPieceSpawnedState(int numClearedRows,
                                                    const Pht::Vec2& scoreTextPosition);
        void OnClearedFilledRowsInCascadingState(int numClearedRows,
                                                 const Pht::Vec2& scoreTextPosition);
        void OnClearedFiveRows(const Pht::Vec2& scoreTextPosition);
        void OnClearedFourRows(const Pht::Vec2& scoreTextPosition);
        void DetectCascade();
        
        enum class State {
            PieceSpawned,
            Cascading,
            Inactive
        };

        Pht::IEngine& mEngine;
        IGameLogic& mGameLogic;
        MediumText& mMediumText;
        EffectManager& mEffectManager;
        State mState {State::Inactive};
        int mNumCombos {0};
        int mPreviousNumCombos {0};
        int mNumCascades {0};
    };
}

#endif
