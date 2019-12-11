#include "FieldExplosionsStates.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Field.hpp"
#include "FieldGravity.hpp"
#include "ScoreManager.hpp"
#include "EffectManager.hpp"
#include "FlyingBlocksAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto bombExplosionMaxReach = 2;
    constexpr auto bombExplosionForceSpeed = 50.0f;
    constexpr auto bombExplosiveForceMagnitude = 30.0f;
    constexpr auto laserCuttingSpeed = 39.0f;
    constexpr auto levelBombExplosionMaxReach = 1;
    constexpr auto levelBombExplosionForceSpeed = 8.0f;
    constexpr auto levelBombExplosiveForceMagnitude = 20.0f;
    constexpr auto bigBombExplosionMaxReach = 2;
    constexpr auto bigBombExplosionForceSpeed = 50.0f;
    constexpr auto bigBombExplosiveForceMagnitude = 40.0f;

    bool CompareRowsTopToDown(int rowIndexA, int rowIndexB) {
        return rowIndexA > rowIndexB;
    }
    
    float ToNumBlocks(BlockKind blockKind) {
        switch (blockKind) {
            case BlockKind::Full:
                return 1.0f;
            case BlockKind::LowerRightHalf:
            case BlockKind::UpperRightHalf:
            case BlockKind::UpperLeftHalf:
            case BlockKind::LowerLeftHalf:
                return 0.5f;
            default:
                return 0.0f;
        }
    }
}

FieldExplosionsStates::FieldExplosionsStates(Pht::IEngine& engine,
                                             Field& field,
                                             FieldGravitySystem& fieldGravity,
                                             ScoreManager& scoreManager,
                                             EffectManager& effectManager,
                                             FlyingBlocksAnimation& flyingBlocksAnimation) :
    mEngine {engine},
    mField {field},
    mFieldGravity {fieldGravity},
    mScoreManager {scoreManager},
    mEffectManager {effectManager},
    mFlyingBlocksAnimation {flyingBlocksAnimation} {}

void FieldExplosionsStates::Init() {
    mExplosionsStates.Clear();
    mRowsToRemove.Clear();
}

FieldExplosionsStates::State FieldExplosionsStates::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    for (auto i = 0; i < mExplosionsStates.Size();) {
        if (UpdateExplosionState(mExplosionsStates.At(i), dt) == State::Active) {
            ++i;
        } else {
            mExplosionsStates.Erase(i);
        }
    }
    
    if (mExplosionsStates.IsEmpty()) {
        RemoveRows();
        return State::Inactive;
    }
    
    return State::Active;
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateExplosionState(ExplosionState& explosionState, float dt) {
    switch (explosionState.mKind) {
        case ExplosionState::Kind::Bomb:
            return UpdateBombExplosionState(explosionState, dt);
        case ExplosionState::Kind::Laser:
            return UpdateRowBombLaserState(explosionState, dt);
        case ExplosionState::Kind::LevelBomb:
            return UpdateLevelBombExplosionState(explosionState, dt);
        case ExplosionState::Kind::BigBomb:
            return UpdateBigBombExplosionState(explosionState, dt);
    }
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateBombExplosionState(ExplosionState& explosionState, float dt) {
    auto removeCorners = false;
    return UpdateGenericBombExplosionState(explosionState,
                                           bombExplosiveForceMagnitude,
                                           bombExplosionForceSpeed,
                                           bombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateRowBombLaserState(ExplosionState& state, float dt) {
    auto& laserState = state.mLaserState;
    auto previousLeftColumn = static_cast<int>(laserState.mLeftCuttingProgress.mXPosition);
    auto previousRightColumn = static_cast<int>(laserState.mRightCuttingProgress.mXPosition);
    auto row = state.mPosition.y;
    
    UpdateLeftCuttingProgress(laserState, row, dt);
    UpdateRightCuttingProgress(laserState, row, dt);

    auto leftColumn = static_cast<int>(laserState.mLeftCuttingProgress.mXPosition);
    auto rightColumn = static_cast<int>(laserState.mRightCuttingProgress.mXPosition);
    if (previousLeftColumn != leftColumn || previousRightColumn != rightColumn) {
        RemoveBlocksHitByLaser(state, previousLeftColumn, previousRightColumn, row);
    }

    if (laserState.mLeftCuttingProgress.mState == LaserState::CuttingState::Done &&
        laserState.mRightCuttingProgress.mState == LaserState::CuttingState::Done) {

        RemoveBlocksHitByLaser(state, leftColumn, rightColumn, row);
        mScoreManager.OnLaserFinished(state.mNumClearedBlocks, state.mPosition);
        return State::Inactive;
    }
    
    return State::Active;
}

void FieldExplosionsStates::RemoveBlocksHitByLaser(ExplosionState& state,
                                                   int leftColumn,
                                                   int rightColumn,
                                                   int row) {
    Pht::IVec2 areaPosition {leftColumn, row};
    Pht::IVec2 areaSize {rightColumn - leftColumn + 1, 1};
    auto removeCorners = true;
    auto removedSubCells = mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners);

    if (removedSubCells.Size() > 0) {
        for (auto i = 0; i < removedSubCells.Size();) {
            auto& subCell = removedSubCells.At(i);
            
            switch (subCell.mBlockKind) {
                case BlockKind::Bomb:
                    DetonateLevelBomb(subCell.mGridPosition);
                    removedSubCells.Erase(i);
                    break;
                default:
                    state.mNumClearedBlocks += ToNumBlocks(subCell.mBlockKind);
                    ++i;
                    break;
            }
        }
        
        mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
    }
}

void FieldExplosionsStates::UpdateLeftCuttingProgress(LaserState& laserState, int row, float dt) {
    auto& cuttingProgress = laserState.mLeftCuttingProgress;

    switch (cuttingProgress.mState) {
        case LaserState::CuttingState::FreeSpace: {
            for (auto column = static_cast<int>(cuttingProgress.mXPosition);;) {
                if (column < 0) {
                    cuttingProgress.mState = LaserState::CuttingState::Done;
                    break;
                }
                if (!mField.GetCell(row, column).IsEmpty()) {
                    cuttingProgress.mXPosition = static_cast<float>(column) + 0.9f;
                    cuttingProgress.mState = LaserState::CuttingState::Cutting;
                    break;
                }
                --column;
            }
            break;
        }
        case LaserState::CuttingState::Cutting: {
            cuttingProgress.mXPosition -= laserCuttingSpeed * dt;
            auto column = static_cast<int>(cuttingProgress.mXPosition);
            if (column < 0) {
                cuttingProgress.mState = LaserState::CuttingState::Done;
                return;
            }
            if (mField.GetCell(row, column).IsEmpty()) {
                cuttingProgress.mState = LaserState::CuttingState::FreeSpace;
            }
            break;
        }
        case LaserState::CuttingState::Done:
            break;
    }
}

void FieldExplosionsStates::UpdateRightCuttingProgress(LaserState& laserState, int row, float dt) {
    auto& cuttingProgress = laserState.mRightCuttingProgress;

    switch (cuttingProgress.mState) {
        case LaserState::CuttingState::FreeSpace: {
            for (auto column = static_cast<int>(cuttingProgress.mXPosition);;) {
                if (column >= mField.GetNumColumns()) {
                    cuttingProgress.mState = LaserState::CuttingState::Done;
                    break;
                }
                if (!mField.GetCell(row, column).IsEmpty()) {
                    cuttingProgress.mXPosition = static_cast<float>(column) + 0.1f;
                    cuttingProgress.mState = LaserState::CuttingState::Cutting;
                    break;
                }
                ++column;
            }
            break;
        }
        case LaserState::CuttingState::Cutting: {
            cuttingProgress.mXPosition += laserCuttingSpeed * dt;
            auto column = static_cast<int>(cuttingProgress.mXPosition);
            if (column >= mField.GetNumColumns()) {
                cuttingProgress.mState = LaserState::CuttingState::Done;
                return;
            }
            if (mField.GetCell(row, column).IsEmpty()) {
                cuttingProgress.mState = LaserState::CuttingState::FreeSpace;
            }
            break;
        }
        case LaserState::CuttingState::Done:
            break;
    }
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateLevelBombExplosionState(ExplosionState& explosionState, float dt) {
    auto removeCorners = true;
    return UpdateGenericBombExplosionState(explosionState,
                                           levelBombExplosiveForceMagnitude,
                                           levelBombExplosionForceSpeed,
                                           levelBombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateBigBombExplosionState(ExplosionState& explosionState,
                                                   float dt) {
    auto removeCorners = true;
    return UpdateGenericBombExplosionState(explosionState,
                                           bigBombExplosiveForceMagnitude,
                                           bigBombExplosionForceSpeed,
                                           bigBombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateGenericBombExplosionState(ExplosionState& state,
                                                       float explosiveForceMagnitude,
                                                       float explosionForceSpeed,
                                                       float explosionMaxReach,
                                                       bool removeCorners,
                                                       float dt) {
    auto& explosionState = state.mBombExplosionState;
    auto previousElapsedTime = explosionState.mElapsedTime;
    explosionState.mElapsedTime += dt;
    
    auto explosionForceReach =
        static_cast<int>(explosionState.mElapsedTime * explosionForceSpeed);
    
    auto previousExplosionForceReach =
        static_cast<int>(previousElapsedTime * explosionForceSpeed);
    
    if (explosionForceReach != previousExplosionForceReach && explosionForceReach > 0) {
        if (explosionForceReach > explosionMaxReach) {
            explosionForceReach = explosionMaxReach;
        }
        
        Pht::IVec2 areaPosition {
            state.mPosition.x - explosionForceReach,
            state.mPosition.y - explosionForceReach
        };
        
        Pht::IVec2 areaSize {explosionForceReach * 2 + 1, explosionForceReach * 2 + 1};
        auto removedSubCells = mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners);
        
        if (removedSubCells.Size() > 0) {
            for (auto i = 0; i < removedSubCells.Size();) {
                auto& subCell = removedSubCells.At(i);
                
                switch (subCell.mBlockKind) {
                    case BlockKind::Bomb:
                        DetonateLevelBomb(subCell.mGridPosition);
                        removedSubCells.Erase(i);
                        break;
                    case BlockKind::RowBomb: {
                        Pht::Vec2 exactPosition {
                            static_cast<float>(subCell.mGridPosition.x),
                            static_cast<float>(subCell.mGridPosition.y)
                        };
                        DetonateRowBomb(subCell.mGridPosition, exactPosition);
                        removedSubCells.Erase(i);
                        break;
                    }
                    default:
                        state.mNumClearedBlocks += ToNumBlocks(subCell.mBlockKind);
                        ++i;
                        break;
                }
            }
            
            auto shouldApplyForceToAlreadyFlyingBlocks =
                explosionState.mShouldApplyForceToAlreadyFlyingBlocks;
            
            mFlyingBlocksAnimation.AddBlocksRemovedByExplosion(removedSubCells,
                                                               state.mPosition,
                                                               explosiveForceMagnitude,
                                                               shouldApplyForceToAlreadyFlyingBlocks);
            explosionState.mShouldApplyForceToAlreadyFlyingBlocks = false;
        }
        
        if (explosionForceReach == explosionMaxReach) {
            mScoreManager.OnBombExplosionFinished(state.mNumClearedBlocks, state.mPosition);
            return State::Inactive;
        }
    }

    return State::Active;
}

void FieldExplosionsStates::RemoveRows() {
    Pht::StaticVector<int, Field::maxNumRows> rowsToRemoveContainingAsteroidCells;
    
    for (auto i = 0; i < mRowsToRemove.Size();) {
        auto rowToRemove = mRowsToRemove.At(i);
        if (RowContainsAsteroid(rowToRemove)) {
            rowsToRemoveContainingAsteroidCells.PushBack(rowToRemove);
            mRowsToRemove.Erase(i);
        } else {
            ++i;
        }
    }
    
    mRowsToRemove.Sort(CompareRowsTopToDown);

    for (auto rowToRemove: mRowsToRemove) {
        for (auto& asteroidRowToRemove: rowsToRemoveContainingAsteroidCells) {
            if (rowToRemove < asteroidRowToRemove) {
                --asteroidRowToRemove;
            }
        }
        
        mField.RemoveRow(rowToRemove);
    }
    
    mRowsToRemove.Clear();
    rowsToRemoveContainingAsteroidCells.Sort(CompareRowsTopToDown);
    
    // The operation of removing (or rather shifting) the rows containing asteroid cells have to
    // came after removing the rows not containing asteroid cells. This is because the shift
    // operation involves pulling down pieces and that cannot be done while removing rows since
    // that would cause some pulled down pieces to be removed unintentionally.
    for (auto asteroidRowToRemove: rowsToRemoveContainingAsteroidCells) {
        // Cannot remove the row the normal way since the asteroid is not removed. Instead, shift
        // the field down wherever possible.
        mFieldGravity.ShiftFieldDown(asteroidRowToRemove);
    }
}

bool FieldExplosionsStates::RowContainsAsteroid(int row) {
    for (auto column = 0; column < mField.GetNumColumns(); ++column) {
        if (mField.GetCell(row, column).mFirstSubCell.IsAsteroid()) {
            return true;
        }
    }
    
    return false;
}

void FieldExplosionsStates::DetonateBomb(const Pht::IVec2& position,
                                         const Pht::Vec2& exactPosition) {
    mEffectManager.StartExplosion(exactPosition);
    
    auto removeCorners = true;
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::Bomb,
        .mPosition = position
    };
    mExplosionsStates.PushBack(explosionState);
}

void FieldExplosionsStates::DetonateRowBomb(const Pht::IVec2& position,
                                            const Pht::Vec2& exactPosition) {
    mEffectManager.StartLaser(exactPosition);
    
    auto removeCorners = true;
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    auto cuttingPositionX = static_cast<float>(position.x) + 0.5f;
    LaserState laserState {
        .mLeftCuttingProgress = LaserState::CuttingProgress{cuttingPositionX},
        .mRightCuttingProgress = LaserState::CuttingProgress{cuttingPositionX}
    };
    
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::Laser,
        .mPosition = position,
        .mLaserState = laserState
    };
    mExplosionsStates.PushBack(explosionState);
    
    if (mRowsToRemove.Find(position.y) == nullptr) {
        mRowsToRemove.PushBack(position.y);
    }
}

void FieldExplosionsStates::DetonateRowBomb(const Pht::IVec2& position) {
    Pht::Vec2 effectPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
    DetonateRowBomb(position, effectPosition);
}

void FieldExplosionsStates::DetonateLevelBomb(const Pht::IVec2& position) {
    Pht::Vec2 effectPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
    mEffectManager.StartLevelBombExplosion(effectPosition);
    
    auto removeCorners = true;
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::LevelBomb,
        .mPosition = position
    };
    mExplosionsStates.PushBack(explosionState);
}

void FieldExplosionsStates::DetonateBigBomb(const Pht::IVec2& position) {
    Pht::Vec2 effectPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
    mEffectManager.StartBigExplosion(effectPosition);
    
    auto removeCorners = true;
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    mField.RemoveAreaOfSubCells(position - Pht::IVec2{0, 1}, {1, 1}, removeCorners);
    
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::BigBomb,
        .mPosition = position
    };
    mExplosionsStates.PushBack(explosionState);
}
