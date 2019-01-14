#include "ComboDetector.hpp"

// Engine includes.
#include "Optional.hpp"
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "SmallTextAnimation.hpp"
#include "EffectManager.hpp"
#include "AudioResources.hpp"

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

ComboDetector::ComboDetector(Pht::IEngine& engine,
                             SmallTextAnimation& smallTextAnimation,
                             EffectManager& effectManager) :
    mEngine {engine},
    mSmallTextAnimation {smallTextAnimation},
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
    PlayClearBlocksSound(mEngine);

    switch (mState) {
        case State::PieceSpawned:
            OnClearedFilledRowsInPieceSpawnedState(removedSubCells);
            break;
        case State::Cascading:
            OnClearedFilledRowsInCascadingState(removedSubCells);
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
        mSmallTextAnimation.StartFantasticMessage();
        mEffectManager.StartSmallCameraShake();
    } else if (numRemovedRows >= 4) {
        mSmallTextAnimation.StartAwesomeMessage();
    } else if (mNumConsecutiveRowClearMoves >= 2) {
        auto numCombos {mNumConsecutiveRowClearMoves - 1};
        mSmallTextAnimation.StartComboMessage(numCombos);
    }
    
    GoToCascadingState();
}

void ComboDetector::OnClearedFilledRowsInCascadingState(const Field::RemovedSubCells& removedSubCells) {
    auto numRemovedRows {CalcNumRemovedRows(removedSubCells)};

    if (numRemovedRows >= 5) {
        mSmallTextAnimation.StartFantasticMessage();
        mEffectManager.StartSmallCameraShake();
    } else if (numRemovedRows >= 4) {
        mSmallTextAnimation.StartAwesomeMessage();
    }
}

void ComboDetector::DetectCascade() {
    if (mNumCascades >= 3) {
        mSmallTextAnimation.StartFantasticMessage();
    } else if (mNumCascades >= 2) {
        mSmallTextAnimation.StartAwesomeMessage();
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