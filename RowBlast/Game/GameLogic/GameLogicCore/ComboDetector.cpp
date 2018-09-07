#include "ComboDetector.hpp"

// Engine includes.
#include "Optional.hpp"

// Game includes.
#include "ComboTextAnimation.hpp"
#include "EffectManager.hpp"

using namespace RowBlast;

namespace {
    int CalcNumRemovedRows(const Field::RemovedSubCells& removedSubCells) {
        Pht::Optional<int> previousRowIndex;
        auto numRemovedRows {0};
        
        for (auto& subCell: removedSubCells) {
            auto rowIndex {subCell.mGridPosition.y};
            
            if (previousRowIndex.HasValue()) {
                if (previousRowIndex.GetValue() != rowIndex) {
                    previousRowIndex = rowIndex;
                    ++numRemovedRows;
                }
            } else {
                previousRowIndex = rowIndex;
                numRemovedRows = 1;
            }
        }
        
        return numRemovedRows;
    }
}

ComboDetector::ComboDetector(ComboTextAnimation& comboTextAnimation, EffectManager& effectManager) :
    mComboTextAnimation {comboTextAnimation},
    mEffectManager {effectManager} {}

void ComboDetector::Init() {
    mState = State::Inactive;
    mNumConsecutiveRowClearMoves = 0;
    mNumCascades = 0;
}

void ComboDetector::OnSpawnPiece() {
    DetectCascade();
    
    mState = State::PieceSpawned;
    mNumCascades = 0;
}

void ComboDetector::OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells) {
    switch (mState) {
        case State::PieceSpawned:
            OnClearedFilledRowsInPieceSpawnedState(removedSubCells);
            break;
        case State::Cascading:
            ++mNumCascades;
            break;
        case State::Inactive:
            break;
    }
}

void ComboDetector::OnClearedFilledRowsInPieceSpawnedState(const Field::RemovedSubCells& removedSubCells) {
    ++mNumConsecutiveRowClearMoves;
    
    auto numRemovedRows {CalcNumRemovedRows(removedSubCells)};

    if (numRemovedRows >= 5) {
        mComboTextAnimation.StartFantasticMessage();
        mEffectManager.StartSmallCameraShake();
    } else if (numRemovedRows >= 4) {
        mComboTextAnimation.StartAwesomeMessage();
    } else if (mNumConsecutiveRowClearMoves >= 2) {
        auto numCombos {mNumConsecutiveRowClearMoves - 1};
        mComboTextAnimation.StartComboMessage(numCombos);
    }
    
    GoToCascadingState();
}

void ComboDetector::DetectCascade() {
    if (mNumCascades >= 3) {
        mComboTextAnimation.StartFantasticMessage();
    } else if (mNumCascades >= 2) {
        mComboTextAnimation.StartAwesomeMessage();
    }
}

void ComboDetector::OnClearedNoFilledRows() {
    mNumConsecutiveRowClearMoves = 0;
}

void ComboDetector::OnUndoMove() {
    if (mNumConsecutiveRowClearMoves > 0) {
        --mNumConsecutiveRowClearMoves;
    }
}

void ComboDetector::GoToCascadingState() {
    mState = State::Cascading;
}
