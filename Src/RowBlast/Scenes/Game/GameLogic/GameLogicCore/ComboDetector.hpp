#ifndef ComboDetector_hpp
#define ComboDetector_hpp

// Game includes.
#include "Field.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class SmallText;
    class EffectManager;

    class ComboDetector {
    public:
        ComboDetector(Pht::IEngine& engine,
                      SmallText& smallTextAnimation,
                      EffectManager& effectManager);
        
        void Init();
        void OnSpawnPiece();
        void OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells);
        void OnClearedNoFilledRows();
        void OnUndoMove();
        void GoToCascadingState();
        
    private:
        void OnClearedFilledRowsInPieceSpawnedState(const Field::RemovedSubCells& removedSubCells);
        void OnClearedFilledRowsInCascadingState(const Field::RemovedSubCells& removedSubCells);
        void DetectCascade();
        
        enum class State {
            PieceSpawned,
            Cascading,
            Inactive
        };

        Pht::IEngine& mEngine;
        SmallText& mSmallText;
        EffectManager& mEffectManager;
        State mState {State::Inactive};
        int mNumConsecutiveRowClearMoves {0};
        int mNumCascades {0};
    };
}

#endif
