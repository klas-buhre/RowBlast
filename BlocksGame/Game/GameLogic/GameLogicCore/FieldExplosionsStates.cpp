#include "FieldExplosionsStates.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Field.hpp"
#include "EffectManager.hpp"
#include "FlyingBlocksAnimation.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto bombExplosionMaxReach {2};
    constexpr auto bombExplosionForceSpeed {50.0f}; // {25.0f};
    constexpr auto laserCuttingSpeed {25.0f};
    constexpr auto levelBombExplosionMaxReach {1};
    constexpr auto levelBombExplosionForceSpeed {25.0f};

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
    }
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateBombExplosionState(BombExplosionState& bombExplosionState, float dt) {
    return UpdateGenericBombExplosionState(bombExplosionState,
                                           bombExplosionForceSpeed,
                                           bombExplosionMaxReach,
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
    
    if (previousLeftColumn != leftColumn || previousRightColumn != rightColumn) {
        auto row {laserState.mCuttingPositionY};
        Pht::IVec2 areaPosition {previousLeftColumn, row};
        Pht::IVec2 areaSize {previousRightColumn - previousLeftColumn + 1, 1};
        auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize)};
        
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

    if (laserState.mLeftCuttingProgress.mState == LaserState::CuttingState::Done &&
        laserState.mRightCuttingProgress.mState == LaserState::CuttingState::Done) {

        return State::Inactive;
    }
    
    return State::Active;
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

/*
FieldExplosionsStates::State
FieldExplosionsStates::UpdateBigLaserState(LaserState& laserState, float dt) {
    auto previousElapsedTime {laserState.mElapsedTime};
    laserState.mElapsedTime += dt;
    
    auto laserForceReach {static_cast<int>(laserState.mElapsedTime * laserForceSpeed)};
    auto previousLaserForceReach {static_cast<int>(previousElapsedTime * laserForceSpeed)};
    
    if (laserForceReach != previousLaserForceReach && laserForceReach > 0) {
        Pht::IVec2 areaPosition {
            laserState.mPosition.x - laserForceReach,
            laserState.mPosition.y - 1
        };
 
        Pht::IVec2 areaSize {laserForceReach * 2 + 1, 3};
        auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize)};

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

        if (laserState.mPosition.x + laserForceReach >= mField.GetNumColumns() - 1 &&
            laserState.mPosition.x - laserForceReach <= 0) {
            
            return State::Inactive;
        }
    }

    return State::Active;
}
*/

FieldExplosionsStates::State
FieldExplosionsStates::UpdateLevelBombExplosionState(BombExplosionState& levelBombExplosionState,
                                                     float dt) {
    return UpdateGenericBombExplosionState(levelBombExplosionState,
                                           levelBombExplosionForceSpeed,
                                           levelBombExplosionMaxReach,
                                           dt);
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateGenericBombExplosionState(BombExplosionState& explosionState,
                                                       float explosionForceSpeed,
                                                       float explosionMaxReach,
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
        auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize)};
        
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
            
            mFlyingBlocksAnimation.AddBlocks(removedSubCells, explosionState.mPosition);
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
    
    mField.RemoveAreaOfSubCells(position, {1, 1});
    
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
    
    mField.RemoveAreaOfSubCells(position, {1, 1});
    
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
    
    mField.RemoveAreaOfSubCells(position, {1, 1});
    
    BombExplosionState bombExplosionState {position};
    ExplosionState explosionState {
        .mKind = ExplosionState::Kind::LevelBomb,
        .mBombExplosionState = bombExplosionState
    };
    mExplosionsStates.PushBack(explosionState);
}
