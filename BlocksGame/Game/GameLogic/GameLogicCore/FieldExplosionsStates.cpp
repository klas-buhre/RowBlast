#include "FieldExplosionsStates.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "Field.hpp"
#include "LaserParticleEffect.hpp"
#include "FlyingBlocksAnimation.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto laserForceSpeed {20.0f};
    
    bool CompareRowsTopToDown(int rowIndexA, int rowIndexB) {
        return rowIndexA > rowIndexB;
    }
}

FieldExplosionsStates::FieldExplosionsStates(Pht::IEngine& engine,
                                             Field& field,
                                             LaserParticleEffect& laserParticleEffect,
                                             FlyingBlocksAnimation& flyingBlocksAnimation) :
    mEngine {engine},
    mField {field},
    mLaserParticleEffect {laserParticleEffect},
    mFlyingBlocksAnimation {flyingBlocksAnimation} {}

FieldExplosionsStates::State FieldExplosionsStates::Update() {
    auto state {State::Inactive};
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto i {0}; i < mExplosionsStates.Size();) {
        if (UpdateExplosionState(mExplosionsStates.At(i), dt) == State::Active) {
            state = State::Active;
            ++i;
        } else {
            mExplosionsStates.Erase(i);
        }
    }
    
    if (state == State::Inactive) {
        RemoveRows();
    }
    
    return state;
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateExplosionState(ExplosionState& explosionState, float dt) {
    switch (explosionState.mKind) {
        case ExplosionState::Kind::Bomb:
            return State::Inactive;
        case ExplosionState::Kind::Laser:
            return UpdateRowBombLaserState(explosionState, dt);
        case ExplosionState::Kind::LevelBomb:
            return State::Inactive;
    }
}

FieldExplosionsStates::State
FieldExplosionsStates::UpdateRowBombLaserState(ExplosionState& laserState, float dt) {
    auto previousElapsedTime {laserState.mElapsedTime};
    laserState.mElapsedTime += dt;
    
    auto laserForceReach {static_cast<int>(laserState.mElapsedTime * laserForceSpeed)};
    auto previousLaserForceReach {static_cast<int>(previousElapsedTime * laserForceSpeed)};
    
    if (laserForceReach != previousLaserForceReach && laserForceReach > 0) {
        Pht::IVec2 areaPosition {laserState.mPosition.x - laserForceReach, laserState.mPosition.y};
        Pht::IVec2 areaSize {laserForceReach * 2 + 1, 1};
        auto removedSubCells {mField.RemoveAreaOfSubCells(areaPosition, areaSize)};
        
        if (removedSubCells.Size() > 0) {
            mFlyingBlocksAnimation.AddBlockRows(removedSubCells);
        }
        
        for (auto& subCell: removedSubCells) {
            if (subCell.mBlockKind == BlockKind::Bomb) {
                DetonateLevelBomb(subCell.mGridPosition);
            }
        }

        if (laserState.mPosition.x + laserForceReach >= mField.GetNumColumns() - 1 &&
            laserState.mPosition.x - laserForceReach <= 0) {
            
            return State::Inactive;
        }
    }

    return State::Inactive;
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

}

void FieldExplosionsStates::DetonateRowBomb(const Pht::IVec2& position,
                                            const Pht::Vec2& exactPosition) {
    mLaserParticleEffect.StartLaser(exactPosition);
    
    mField.RemoveAreaOfSubCells(position, {1, 1});
    
    ExplosionState laserState {ExplosionState::Kind::Laser, position};
    mExplosionsStates.PushBack(laserState);
    
    if (mRowsToRemove.Find(position.y) == nullptr) {
        mRowsToRemove.PushBack(position.y);
    }
}

void FieldExplosionsStates::DetonateLevelBomb(const Pht::IVec2& position) {

}
