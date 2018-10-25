#ifndef ComboDetector_hpp
#define ComboDetector_hpp

// Game includes.
#include "Field.hpp"

namespace RowBlast {
    class ComboTextAnimation;
    class EffectManager;

    class ComboDetector {
    public:
        ComboDetector(ComboTextAnimation& comboTextAnimation, EffectManager& effectManager);
        
        void Init();
        void OnSpawnPiece();
        void OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells);
        void OnClearedNoFilledRows();
        void OnUndoMove();
        void GoToCascadingState();
        
    private:
        void OnClearedFilledRowsInPieceSpawnedState(const Field::RemovedSubCells& removedSubCells);
        void DetectCascade();
        
        enum class State {
            PieceSpawned,
            Cascading,
            Inactive
        };

        ComboTextAnimation& mComboTextAnimation;
        EffectManager& mEffectManager;
        State mState {State::Inactive};
        int mNumConsecutiveRowClearMoves {0};
        int mNumCascades {0};
    };
}

#endif
