#ifndef FieldExplosionsStates_hpp
#define FieldExplosionsStates_hpp

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Field;
    class EffectManager;
    class FlyingBlocksAnimation;
    
    class FieldExplosionsStates {
    public:
        enum class State {
            Active,
            Inactive
        };

        FieldExplosionsStates(Pht::IEngine& engine,
                              Field& field,
                              EffectManager& effectManager,
                              FlyingBlocksAnimation& flyingBlocksAnimation);
        
        State Update();
        void DetonateBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position);
        void DetonateLevelBomb(const Pht::IVec2& position);
        void DetonateBigBomb(const Pht::IVec2& position);

    private:
        struct BombExplosionState {
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
            bool mShouldApplyForceToAlreadyFlyingBlocks {true};
        };
        
        struct LaserState {
            enum class CuttingState {
                FreeSpace,
                Cutting,
                Done
            };

            struct CuttingProgress {
                float mXPosition;
                CuttingState mState {CuttingState::FreeSpace};
            };
            
            CuttingProgress mLeftCuttingProgress;
            CuttingProgress mRightCuttingProgress;
            int mCuttingPositionY;
        };
        
        struct ExplosionState {
            enum class Kind {
                Bomb,
                Laser,
                LevelBomb,
                BigBomb
            };
            
            Kind mKind;
            BombExplosionState mBombExplosionState;
            LaserState mLaserState;
        };
        
        using ExplosionsStates = Pht::StaticVector<ExplosionState, 200>;
        
        State UpdateExplosionState(ExplosionState& explosionState, float dt);
        State UpdateBombExplosionState(BombExplosionState& bombExplosionState, float dt);
        State UpdateRowBombLaserState(LaserState& laserState, float dt);
        void RemoveBlocksHitByLaser(int leftColumn, int rightColumn, int row);
        void UpdateLeftCuttingProgress(LaserState& laserState, float dt);
        void UpdateRightCuttingProgress(LaserState& laserState, float dt);
        State UpdateLevelBombExplosionState(BombExplosionState& levelBombExplosionState, float dt);
        State UpdateBigBombExplosionState(BombExplosionState& bigBombExplosionState, float dt);
        State UpdateGenericBombExplosionState(BombExplosionState& explosionState,
                                              float explosiveForceMagnitude,
                                              float explosionForceSpeed,
                                              float explosionMaxReach,
                                              bool removeCorners,
                                              float dt);
        void RemoveRows();
        bool RowContainsAsteroid(int row);
        
        Pht::IEngine& mEngine;
        Field& mField;
        EffectManager& mEffectManager;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        ExplosionsStates mExplosionsStates;
        Pht::StaticVector<int, 10> mRowsToRemove;
    };
}

#endif
