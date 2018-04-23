#include "FieldExplosionsStates.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Field.hpp"
#include "EffectManager.hpp"
#include "FlyingBlocksAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto bombExplosionMaxReach {2};
    constexpr auto bombExplosionForceSpeed {50.0f};
    constexpr auto bombExplosiveForceMagnitude {30.0f};
    constexpr auto laserCuttingSpeed {39.0f};
    constexpr auto levelBombExplosionMaxReach {1};
    constexpr auto levelBombExplosionForceSpeed {12.0f};
    constexpr auto levelBombExplosiveForceMagnitude {20.0f};
    constexpr auto bigBombExplosionMaxReach {2};
    constexpr auto bigBombExplosionForceSpeed {50.0f};
    constexpr auto bigBombExplosiveForceMagnitude {40.0f};

    bool CompareRowsTopToDown(int rowIndexA, int rowIndexB) {
        return rowIndexA > rowIndexB;
    }
}

FieldExplosionsStates::FieldExplosionsStates(Pht::IEngine& engine,
                                             Field& field,
                                             EffectManager& effectManager,
                                             FlyingBlocksAnimation& flyingBlocksAnimation) :
    mEngine {engine},
    mField {field},
    mEffectManager {effectManager},
    mFlyingBlocksAnimation {flyingBlocksAnimation} {}

FieldExplosionsStates::State FieldExplosionsStates::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto i {0}; i < mExplosionsStates.Size();) {
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
            return UpdateBombExplosionState(explosionState.mBombExplosionState, dt);
        case ExplosionState::Kind::Laser:
            return UpdateRowBombLaserState(explosionState.mLaserState, dt);
        case ExplosionState::Kind::LevelBomb:
            return UpdateLevelBombExplosionState(explosionState.mBombExplosionState, dt);
        case ExplosionState::Kind::BigBomb:
            return UpdateBigBombExplosionState(explosionState.mBombExplosionState, dt);
    }
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateBombExplosionState(BombExplosionState& bombExplosionState, float dt) {
    auto removeCorners {false};
    return UpdateGenericBombExplosionState(bombExplosionState,
                                           bombExplosiveForceMagnitude,
                                           bombExplosionForceSpeed,
                                           bombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateRowBombLaserState(LaserState& laserState, float dt) {
    auto previousLeftColumn {static_cast<int>(laserState.mLeftCuttingProgress.mXPosition)};
    auto previousRightColumn {static_cast<int>(laserState.mRightCuttingProgress.mXPosition)};
    
    UpdateLeftCuttingProgress(laserState, dt);
    UpdateRightCuttingProgress(laserState, dt);

    auto leftColumn {static_cast<int>(laserState.mLeftCuttingProgress.mXPosition)};
    auto rightColumn {static_cast<int>(laserState.mRightCuttingProgress.mXPosition)};
    auto row {laserState.mCuttingPositionY};
    
    if (previousLeftColumn != leftColumn || previousRightColumn != rightColumn) {
        RemoveBlocksHitByLaser(previousLeftColumn, previousRightColumn, row);
    }

    if (laserState.mLeftCuttingProgress.mState == LaserState::CuttingState::Done &&
        laserState.mRightCuttingProgress.mState == LaserState::CuttingState::Done) {

        RemoveBlocksHitByLaser(leftColumn, rightColumn, row);
        return State::Inactive;
    }
    
    return State::Active;
}

void FieldExplosionsStates::RemoveBlocksHitByLaser(int leftColumn, int rightColumn, int row) {
    Pht::IVec2 areaPosition {leftColumn, row};
    Pht::IVec2 areaSize {rightColumn - leftColumn + 1, 1};
    auto removeCorners {true};
    auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners)};

    if (removedSubCells.Size() > 0) {
        for (auto i {0}; i < removedSubCells.Size();) {
            auto& subCell {removedSubCells.At(i)};
            
            switch (subCell.mBlockKind) {
                case BlockKind::Bomb:
                    DetonateLevelBomb(subCell.mGridPosition);
                    removedSubCells.Erase(i);
                    break;
                default:
                    ++i;
                    break;
            }
        }
        
        mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
    }
}

void FieldExplosionsStates::UpdateLeftCuttingProgress(LaserState& laserState, float dt) {
    auto row {laserState.mCuttingPositionY};
    auto& cuttingProgress {laserState.mLeftCuttingProgress};

    switch (cuttingProgress.mState) {
        case LaserState::CuttingState::FreeSpace: {
            for (auto column {static_cast<int>(cuttingProgress.mXPosition)};;) {
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
            auto column {static_cast<int>(cuttingProgress.mXPosition)};
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

void FieldExplosionsStates::UpdateRightCuttingProgress(LaserState& laserState, float dt) {
    auto row {laserState.mCuttingPositionY};
    auto& cuttingProgress {laserState.mRightCuttingProgress};

    switch (cuttingProgress.mState) {
        case LaserState::CuttingState::FreeSpace: {
            for (auto column {static_cast<int>(cuttingProgress.mXPosition)};;) {
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
            auto column {static_cast<int>(cuttingProgress.mXPosition)};
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
FieldExplosionsStates::UpdateLevelBombExplosionState(BombExplosionState& levelBombExplosionState,
                                                     float dt) {
    auto removeCorners {true};
    return UpdateGenericBombExplosionState(levelBombExplosionState,
                                           levelBombExplosiveForceMagnitude,
                                           levelBombExplosionForceSpeed,
                                           levelBombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateBigBombExplosionState(BombExplosionState& bigBombExplosionState,
                                                   float dt) {
    auto removeCorners {true};
    return UpdateGenericBombExplosionState(bigBombExplosionState,
                                           bigBombExplosiveForceMagnitude,
                                           bigBombExplosionForceSpeed,
                                           bigBombExplosionMaxReach,
                                           removeCorners,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateGenericBombExplosionState(BombExplosionState& explosionState,
                                                       float explosiveForceMagnitude,
                                                       float explosionForceSpeed,
                                                       float explosionMaxReach,
                                                       bool removeCorners,
                                                       float dt) {
    auto previousElapsedTime {explosionState.mElapsedTime};
    explosionState.mElapsedTime += dt;
    
    auto explosionForceReach {
        static_cast<int>(explosionState.mElapsedTime * explosionForceSpeed)
    };
    
    auto previousExplosionForceReach {
        static_cast<int>(previousElapsedTime * explosionForceSpeed)
    };
    
    if (explosionForceReach != previousExplosionForceReach && explosionForceReach > 0) {
        if (explosionForceReach > explosionMaxReach) {
            explosionForceReach = explosionMaxReach;
        }
        
        Pht::IVec2 areaPosition {
            explosionState.mPosition.x - explosionForceReach,
            explosionState.mPosition.y - explosionForceReach
        };
        
        Pht::IVec2 areaSize {explosionForceReach * 2 + 1, explosionForceReach * 2 + 1};
        auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize, removeCorners)};
        
        if (removedSubCells.Size() > 0) {
            for (auto i {0}; i < removedSubCells.Size();) {
                auto& subCell {removedSubCells.At(i)};
                
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
                        ++i;
                        break;
                }
            }
            
            mFlyingBlocksAnimation.AddBlocks(removedSubCells,
                                             explosionState.mPosition,
                                             explosiveForceMagnitude,
                                             explosionState.mShouldApplyForceToAlreadyFlyingBlocks);
            explosionState.mShouldApplyForceToAlreadyFlyingBlocks = false;
        }
        
        if (explosionForceReach == explosionMaxReach) {
            return State::Inactive;
        }
    }

    return State::Active;
}

void FieldExplosionsStates::RemoveRows() {
    mRowsToRemove.Sort(CompareRowsTopToDown);
    
    for (auto i {0}; i < mRowsToRemove.Size(); ++i) {
        mField.RemoveRow(mRowsToRemove.At(i));
    }
    
    mRowsToRemove.Clear();
}

void FieldExplosionsStates::DetonateBomb(const Pht::IVec2& position,
                                         const Pht::Vec2& exactPosition) {
    mEffectManager.StartExplosion(exactPosition);
    
    auto removeCorners {true};
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    BombExplosionState bombExplosionState {position};
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::Bomb,
        .mBombExplosionState = bombExplosionState
    };
    mExplosionsStates.PushBack(explosionState);
}

void FieldExplosionsStates::DetonateRowBomb(const Pht::IVec2& position,
                                            const Pht::Vec2& exactPosition) {
    mEffectManager.StartLaser(exactPosition);
    
    auto removeCorners {true};
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    auto cuttingPositionX {static_cast<float>(position.x) + 0.5f};
    LaserState laserState {
        .mLeftCuttingProgress = LaserState::CuttingProgress{cuttingPositionX},
        .mRightCuttingProgress = LaserState::CuttingProgress{cuttingPositionX},
        .mCuttingPositionY = position.y
    };
    
    ExplosionState explosionState {.mKind = ExplosionState::Kind::Laser, .mLaserState = laserState};
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
    
    auto removeCorners {true};
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    
    BombExplosionState bombExplosionState {position};
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::LevelBomb,
        .mBombExplosionState = bombExplosionState
    };
    mExplosionsStates.PushBack(explosionState);
}

void FieldExplosionsStates::DetonateBigBomb(const Pht::IVec2& position) {
    Pht::Vec2 effectPosition {static_cast<float>(position.x), static_cast<float>(position.y)};
    mEffectManager.StartBigExplosion(effectPosition);
    
    auto removeCorners {true};
    mField.RemoveAreaOfSubCells(position, {1, 1}, removeCorners);
    mField.RemoveAreaOfSubCells(position - Pht::IVec2{0, 1}, {1, 1}, removeCorners);
    
    BombExplosionState bombExplosionState {position};
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::BigBomb,
        .mBombExplosionState = bombExplosionState
    };
    mExplosionsStates.PushBack(explosionState);
}
