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
    
    Pht::Vec2 CalcScoreTextPosition(const Field::RemovedSubCells& removedSubCells,
                                    Pht::Optional<int> landedPieceId) {
        auto yMax = 0;
        auto xMax = 0;
        auto pieceXMax = 0;
        auto pieceXMin = 1000;
        
        for (auto& subCell: removedSubCells) {
            auto cellY = subCell.mGridPosition.y;
            if (cellY > yMax) {
                yMax = cellY;
            }
            
            auto cellX = subCell.mGridPosition.x;
            if (cellX > xMax) {
                xMax = cellX;
            }
            
            if (landedPieceId.HasValue() && subCell.mPieceId == landedPieceId.GetValue()) {
                if (cellX > pieceXMax) {
                    pieceXMax = cellX;
                }
                
                if (cellX < pieceXMin) {
                    pieceXMin = cellX;
                }
            }
        }
        
        auto yPosition = static_cast<float>(yMax) + 0.5f;
        if (landedPieceId.HasValue()) {
            return {static_cast<float>(pieceXMin + pieceXMax) / 2.0f, yPosition};
        }
        
        return {static_cast<float>(xMax) / 2.0f, yPosition};
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

void ScoreManager::OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells,
                                       Pht::Optional<int> landedPieceId) {
    PlayClearBlocksSound(mEngine);
    auto numClearedRows = CalcNumRemovedRows(removedSubCells);
    auto scoreTextPosition = CalcScoreTextPosition(removedSubCells, landedPieceId);

    switch (mState) {
        case State::PieceSpawned:
            OnClearedFilledRowsInPieceSpawnedState(numClearedRows, scoreTextPosition);
            break;
        case State::Cascading:
            OnClearedFilledRowsInCascadingState(numClearedRows, scoreTextPosition);
            ++mNumCascades;
            break;
        case State::Inactive:
            break;
    }
}

void ScoreManager::OnClearedFilledRowsInPieceSpawnedState(int numClearedRows,
                                                          const Pht::Vec2& scoreTextPosition) {
    if (numClearedRows >= 5) {
        OnClearedFiveRows(scoreTextPosition);
    } else if (numClearedRows >= 4) {
        OnClearedFourRows(scoreTextPosition);
    } else {
        auto points = clearOneRowPoints * numClearedRows + mNumCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points, scoreTextPosition);
        if (mNumCombos >= 1) {
            mSmallText.StartComboMessage(mNumCombos);
        }
    }
    
    ++mNumCombos;
    GoToCascadingState();
}

void ScoreManager::OnClearedFilledRowsInCascadingState(int numClearedRows,
                                                       const Pht::Vec2& scoreTextPosition) {
    if (numClearedRows >= 5) {
        OnClearedFiveRows(scoreTextPosition);
    } else if (numClearedRows >= 4) {
        OnClearedFourRows(scoreTextPosition);
    } else {
        auto points = clearOneRowInCascadePoints * numClearedRows + mNumCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points, scoreTextPosition);
    }
}

void ScoreManager::OnClearedFiveRows(const Pht::Vec2& scoreTextPosition) {
    auto points = clearFiveRowsPoints + mNumCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points, scoreTextPosition);
    mSmallText.StartFantasticMessage();
    mEffectManager.StartSmallCameraShake();
}

void ScoreManager::OnClearedFourRows(const Pht::Vec2& scoreTextPosition) {
    auto points = clearFourRowsPoints + mNumCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points, scoreTextPosition);
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

void ScoreManager::OnFilledSlots(int numSlots, const Pht::Vec2& scoreTextPosition) {
    mGameLogic.IncreaseScore(numSlots * fillSlotPoints, scoreTextPosition);
}

void ScoreManager::OnUndoMove() {
    if (mNumCombos > 0) {
        --mNumCombos;
    }
}

void ScoreManager::GoToCascadingState() {
    mState = State::Cascading;
}

int ScoreManager::CalculateExtraPoints(int currentScore,
                                       int numMovesLeft,
                                       int numMovesAtLevelStart) {
    auto factor = static_cast<float>(numMovesLeft) / static_cast<float>(numMovesAtLevelStart);
    return factor * static_cast<float>(currentScore);
}
