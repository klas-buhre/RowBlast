#include "ScoreManager.hpp"

// Engine includes.
#include "Optional.hpp"
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes.
#include "SmallText.hpp"
#include "EffectManager.hpp"
#include "AudioResources.hpp"
#include "IGameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto clearOneRowPoints = 60;
    constexpr auto clearOneRowInCascadePoints = 50;
    constexpr auto comboIncreasePoints = 40;
    constexpr auto clearFiveRowsPoints = 2000;
    constexpr auto clearFourRowsPoints = 1000;
    constexpr auto fillSlotPoints = 15;
    
    int CalcNumRemovedRows(const Field::RemovedSubCells& removedSubCells) {
        Pht::Optional<int> previousRowIndex;
        auto numRemovedRows = 0;
        
        for (auto& subCell: removedSubCells) {
            auto rowIndex = subCell.mGridPosition.y;
            
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

ScoreManager::ScoreManager(Pht::IEngine& engine,
                           IGameLogic& gameLogic,
                           SmallText& smallTextAnimation,
                           EffectManager& effectManager) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mSmallText {smallTextAnimation},
    mEffectManager {effectManager} {}

void ScoreManager::Init() {
    mState = State::Inactive;
    mNumCombos = 0;
    mNumCascades = 0;
}

void ScoreManager::OnSpawnPiece() {
    DetectCascade();
    
    mState = State::PieceSpawned;
    mNumCascades = 0;
}

void ScoreManager::OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells) {
    PlayClearBlocksSound(mEngine);
    auto numClearedRows = CalcNumRemovedRows(removedSubCells);

    switch (mState) {
        case State::PieceSpawned:
            OnClearedFilledRowsInPieceSpawnedState(numClearedRows);
            break;
        case State::Cascading:
            OnClearedFilledRowsInCascadingState(numClearedRows);
            ++mNumCascades;
            break;
        case State::Inactive:
            break;
    }
}

void ScoreManager::OnClearedFilledRowsInPieceSpawnedState(int numClearedRows) {
    if (numClearedRows >= 5) {
        OnClearedFiveRows();
    } else if (numClearedRows >= 4) {
        OnClearedFourRows();
    } else {
        auto points = clearOneRowPoints * numClearedRows + mNumCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points);
        if (mNumCombos >= 1) {
            mSmallText.StartComboMessage(mNumCombos);
        }
    }
    
    ++mNumCombos;
    GoToCascadingState();
}

void ScoreManager::OnClearedFilledRowsInCascadingState(int numClearedRows) {
    if (numClearedRows >= 5) {
        OnClearedFiveRows();
    } else if (numClearedRows >= 4) {
        OnClearedFourRows();
    } else {
        auto points = clearOneRowInCascadePoints * numClearedRows + mNumCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points);
    }
}

void ScoreManager::OnClearedFiveRows() {
    auto points = clearFiveRowsPoints + mNumCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points);
    mSmallText.StartFantasticMessage();
    mEffectManager.StartSmallCameraShake();
}

void ScoreManager::OnClearedFourRows() {
    auto points = clearFourRowsPoints + mNumCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points);
    mSmallText.StartAwesomeMessage();
}

void ScoreManager::DetectCascade() {
    if (mNumCascades >= 3) {
        mSmallText.StartFantasticMessage();
    } else if (mNumCascades >= 2) {
        mSmallText.StartAwesomeMessage();
    }
}

void ScoreManager::OnClearedNoFilledRows() {
    mNumCombos = 0;
}

void ScoreManager::OnFilledSlots(int numSlots) {
    mGameLogic.IncreaseScore(numSlots * fillSlotPoints);
}

void ScoreManager::OnUndoMove() {
    if (mNumCombos > 0) {
        --mNumCombos;
    }
}

void ScoreManager::GoToCascadingState() {
    mState = State::Cascading;
}
