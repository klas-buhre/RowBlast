#include "ScoreManager.hpp"

// Engine includes.
#include "Optional.hpp"

// Game includes.
#include "MediumText.hpp"
#include "EffectManager.hpp"
#include "AudioResources.hpp"
#include "IGameLogic.hpp"

using namespace RowBlast;

namespace {
    constexpr auto clearBlockPoints = 10;
    constexpr auto clearOneRowInCascadeDeductPoints = 10;
    constexpr auto comboIncreasePoints = 40;
    constexpr auto clearFourRowsIncreasePoints = 800;
    constexpr auto clearFiveRowsIncreasePoints = 1800;
    constexpr auto asteroidIsDownIncreasePoints = 500;
    constexpr auto fillSlotPoints = 10;
    constexpr auto laserScoreTextDelay = 0.275f;
    constexpr auto asteroidIsDownScoreTextDelay = 0.5f;
    
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

ScoreManager::ScoreManager(const Field& field,
                           IGameLogic& gameLogic,
                           MediumText& mediumTextAnimation,
                           EffectManager& effectManager) :
    mField {field},
    mGameLogic {gameLogic},
    mMediumText {mediumTextAnimation},
    mEffectManager {effectManager} {}

void ScoreManager::Init() {
    mState = State::Inactive;
    mAsteroidIsDown = false;
    mNumCombos = 0;
    mPreviousNumCombos = 0;
    mNumCascades = 0;
    mNumPointsFromBombsAndLasersThisMove = 0;
    
    mClearOneRowPoints = clearBlockPoints * mField.GetNumColumns();
    mClearOneRowInCascadePoints = mClearOneRowPoints - clearOneRowInCascadeDeductPoints;
}

void ScoreManager::OnNewMove() {
    if (mNumCascades >= 3) {
        mMediumText.StartFantasticMessage();
    } else if (mNumCascades >= 2) {
        mMediumText.StartAwesomeMessage();
    }
    
    if (mNumPointsFromBombsAndLasersThisMove > 220) {
        mMediumText.StartFantasticMessage();
    } else if (mNumPointsFromBombsAndLasersThisMove > 170) {
        mMediumText.StartAwesomeMessage();
    }
    
    mState = State::NotCascading;
    mNumCascades = 0;
    mNumPointsFromBombsAndLasersThisMove = 0;
}

void ScoreManager::OnClearedFilledRows(const Field::RemovedSubCells& removedSubCells,
                                       Pht::Optional<int> landedPieceId) {
    auto numClearedRows = removedSubCells.Size() / mField.GetNumColumns();
    auto scoreTextPosition = CalcScoreTextPosition(removedSubCells, landedPieceId);

    switch (mState) {
        case State::NotCascading:
            OnClearedFilledRowsInNotCascadingState(numClearedRows, scoreTextPosition);
            break;
        case State::Cascading:
            OnClearedFilledRowsInCascadingState(numClearedRows, scoreTextPosition);
            ++mNumCascades;
            break;
        case State::Inactive:
            break;
    }
}

void ScoreManager::OnClearedFilledRowsInNotCascadingState(int numClearedRows,
                                                          const Pht::Vec2& scoreTextPosition) {
    if (numClearedRows >= 5) {
        OnClearedFiveRows(scoreTextPosition, mNumCombos);
    } else if (numClearedRows >= 4) {
        OnClearedFourRows(scoreTextPosition, mNumCombos);
    } else {
        auto points = mClearOneRowPoints * numClearedRows + mNumCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points, scoreTextPosition);
        if (mNumCombos >= 1) {
            mMediumText.StartDeferredComboMessage(mNumCombos);
        }
    }
    
    mPreviousNumCombos = mNumCombos;
    ++mNumCombos;
    GoToCascadingState();
}

void ScoreManager::OnClearedFilledRowsInCascadingState(int numClearedRows,
                                                       const Pht::Vec2& scoreTextPosition) {
    auto numCombos = mNumCombos > 0 ? mNumCombos - 1 : 0;
    
    if (numClearedRows >= 5) {
        OnClearedFiveRows(scoreTextPosition, numCombos);
    } else if (numClearedRows >= 4) {
        OnClearedFourRows(scoreTextPosition, numCombos);
    } else {
        auto points = mClearOneRowInCascadePoints * numClearedRows +
                      numCombos * comboIncreasePoints;
        mGameLogic.IncreaseScore(points, scoreTextPosition);
    }
}

void ScoreManager::OnClearedFiveRows(const Pht::Vec2& scoreTextPosition, int numCombos) {
    auto points = mClearOneRowPoints * 5 + clearFiveRowsIncreasePoints +
                  numCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points, scoreTextPosition);
    mMediumText.StartFantasticMessage();
    mEffectManager.StartSmallCameraShake();
}

void ScoreManager::OnClearedFourRows(const Pht::Vec2& scoreTextPosition, int numCombos) {
    auto points = mClearOneRowPoints * 4 + clearFourRowsIncreasePoints +
                  numCombos * comboIncreasePoints;
    mGameLogic.IncreaseScore(points, scoreTextPosition);
    mMediumText.StartAwesomeMessage();
}

void ScoreManager::OnClearedNoFilledRows() {
    mPreviousNumCombos = mNumCombos;
    mNumCombos = 0;
}

void ScoreManager::OnBombExplosionFinished(float numBlocksCleared, const Pht::IVec2& gridPosition) {
    if (numBlocksCleared > 0.0f) {
        Pht::Vec2 position {static_cast<float>(gridPosition.x), static_cast<float>(gridPosition.y)};
        auto points = static_cast<int>(numBlocksCleared * clearBlockPoints);
        mNumPointsFromBombsAndLasersThisMove += points;
        mGameLogic.IncreaseScore(points, position);
    }
}

void ScoreManager::OnLaserFinished(float numBlocksCleared, const Pht::IVec2& gridPosition) {
    if (numBlocksCleared > 0.0f) {
        Pht::Vec2 position {static_cast<float>(gridPosition.x), static_cast<float>(gridPosition.y)};
        auto points = static_cast<int>(numBlocksCleared * clearBlockPoints);
        mNumPointsFromBombsAndLasersThisMove += points;
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

void ScoreManager::OnAsteroidIsDown() {
    if (mAsteroidIsDown) {
        return;
    }
    
    mAsteroidIsDown = true;
    auto scoreTextPosition = CalcAsteroidScoreTextPosition();
    if (scoreTextPosition.HasValue()) {
        mGameLogic.IncreaseScore(asteroidIsDownIncreasePoints,
                                 scoreTextPosition.GetValue(),
                                 asteroidIsDownScoreTextDelay);
    }
}

void ScoreManager::OnUndoMove() {
    mNumCombos = mPreviousNumCombos;
}

void ScoreManager::GoToCascadingState() {
    mState = State::Cascading;
}

int ScoreManager::CalculateBonusPointsAtLevelCompleted(int movesLeft) const {
    return mClearOneRowPoints * movesLeft;
}

Pht::Optional<Pht::Vec2> ScoreManager::CalcAsteroidScoreTextPosition() const {
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            switch (mField.GetCell(row, column).mFirstSubCell.mBlockKind) {
                case BlockKind::BigAsteroidMainCell:
                    return Pht::Vec2 {
                        static_cast<float>(column) + 0.5f,
                        static_cast<float>(row) + 1.75f
                    };
                case BlockKind::SmallAsteroid:
                    return Pht::Vec2 {static_cast<float>(column), static_cast<float>(row) + 0.75f};
                default:
                    break;
            }
        }
    }
    
    return Pht::Optional<Pht::Vec2> {};
}
