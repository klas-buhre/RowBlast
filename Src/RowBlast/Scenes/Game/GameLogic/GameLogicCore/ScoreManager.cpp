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
    constexpr auto clearBlockPoints = 10;
    constexpr auto clearOneRowPoints = 60;
    constexpr auto clearOneRowInCascadePoints = 50;
    constexpr auto comboIncreasePoints = 40;
    constexpr auto clearFiveRowsPoints = 2000;
    constexpr auto clearFourRowsPoints = 1000;
    constexpr auto fillSlotPoints = 15;
    constexpr auto laserScoreTextDelay = 0.275f;
    
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
        auto xMin = 1000;
        auto pieceXMax = 0;
        auto pieceXMin = 1000;
        
        for (auto& subCell: removedSubCells) {
            auto cellY = subCell.mGridPosition.y;
            auto cellX = subCell.mGridPosition.x;
            if (cellY > yMax) {
                yMax = cellY;
            }

            if (subCell.mFlags.mIsPulledDown) {
                if (cellX > xMax) {
                    xMax = cellX;
                }
                
                if (cellX < xMin) {
                    xMin = cellX;
                }
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
        
        auto yPosition = static_cast<float>(yMax);
        if (landedPieceId.HasValue()) {
            return {static_cast<float>(pieceXMin + pieceXMax) / 2.0f, yPosition};
        }
        
        return {static_cast<float>(xMin + xMax) / 2.0f, yPosition};
    }
    
    int CalcNumBlueprintSlotsFilled(const Field::PieceFilledSlots& slotsCoveredByPiece) {
        auto numBlueprintSlotsFilled = 0;
        for (auto& slot: slotsCoveredByPiece) {
            if (slot.mKind == PieceFilledSlotKind::Blueprint) {
                ++numBlueprintSlotsFilled;
            }
        }
        
        return numBlueprintSlotsFilled;
    }
    
    Pht::Vec2 CalcScoreTextPosition(const Field::PieceFilledSlots& slotsCoveredByPiece) {
        auto pieceFillYMax = 0;
        auto pieceFillXMax = 0;
        auto pieceFillXMin = 1000;

        for (auto& slot: slotsCoveredByPiece) {
            if (slot.mKind == PieceFilledSlotKind::OutsideBlueprint) {
                continue;
            }
            
            auto slotY = slot.mPosition.y;
            if (slotY > pieceFillYMax) {
                pieceFillYMax = slotY;
            }
            
            auto slotX = slot.mPosition.x;
            if (slotX > pieceFillXMax) {
                pieceFillXMax = slotX;
            }

            if (slotX < pieceFillXMin) {
                pieceFillXMin = slotX;
            }
        }
        
        auto yPosition = static_cast<float>(pieceFillYMax) + 0.5f;
        return {static_cast<float>(pieceFillXMin + pieceFillXMax) / 2.0f, yPosition};
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
    mPreviousNumCombos = 0;
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
    
    mPreviousNumCombos = mNumCombos;
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
    mPreviousNumCombos = mNumCombos;
    mNumCombos = 0;
}

void ScoreManager::OnBombExplosionFinished(float numBlocksCleared, const Pht::IVec2& gridPosition) {
    if (numBlocksCleared > 0.0f) {
        Pht::Vec2 position {static_cast<float>(gridPosition.x), static_cast<float>(gridPosition.y)};
        auto points = static_cast<int>(numBlocksCleared * clearBlockPoints);
        mGameLogic.IncreaseScore(points, position);
    }
}

void ScoreManager::OnLaserFinished(float numBlocksCleared, const Pht::IVec2& gridPosition) {
    if (numBlocksCleared > 0.0f) {
        Pht::Vec2 position {static_cast<float>(gridPosition.x), static_cast<float>(gridPosition.y)};
        auto points = static_cast<int>(numBlocksCleared * clearBlockPoints);
        mGameLogic.IncreaseScore(points, position, laserScoreTextDelay);
    }
}

void ScoreManager::OnFilledSlots(const Field::PieceFilledSlots& slotsCoveredByPiece) {
    auto numBlueprintSlotsFilled = CalcNumBlueprintSlotsFilled(slotsCoveredByPiece);
    if (numBlueprintSlotsFilled > 0) {
        auto scoreTextPosition = CalcScoreTextPosition(slotsCoveredByPiece);
        mGameLogic.IncreaseScore(numBlueprintSlotsFilled * fillSlotPoints, scoreTextPosition);
    }
}

void ScoreManager::OnUndoMove() {
    mNumCombos = mPreviousNumCombos;
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
