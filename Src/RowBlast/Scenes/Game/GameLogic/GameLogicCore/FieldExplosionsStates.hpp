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
    class FieldGravitySystem;
    class ScoreManager;
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
                              FieldGravitySystem& fieldGravity,
                              ScoreManager& scoreManager,
                              EffectManager& effectManager,
                              FlyingBlocksAnimation& flyingBlocksAnimation);
        
        void Init();
        State Update();
        void DetonateBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position);
        void DetonateLevelBomb(const Pht::IVec2& position);
        void DetonateBigBomb(const Pht::IVec2& position);

    private:
        struct BombExplosionState {
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
        };
        
        struct ExplosionState {
            enum class Kind {
                Bomb,
                Laser,
                LevelBomb,
                BigBomb
            };
            
            Kind mKind;
            Pht::IVec2 mPosition;
            int mNumClearedBlocks {0};
            BombExplosionState mBombExplosionState;
            LaserState mLaserState;
        };
        
        using ExplosionsStates = Pht::StaticVector<ExplosionState, 200>;
        
        State UpdateExplosionState(ExplosionState& explosionState, float dt);
        State UpdateBombExplosionState(ExplosionState& explosionState, float dt);
        State UpdateRowBombLaserState(ExplosionState& state, float dt);
        void RemoveBlocksHitByLaser(ExplosionState& state,
                                    int leftColumn,
                                    int rightColumn,
                                    int row);
        void UpdateLeftCuttingProgress(LaserState& laserState, int row, float dt);
        void UpdateRightCuttingProgress(LaserState& laserState, int row, float dt);
        State UpdateLevelBombExplosionState(ExplosionState& explosionState, float dt);
        State UpdateBigBombExplosionState(ExplosionState& explosionState, float dt);
        State UpdateGenericBombExplosionState(ExplosionState& state,
                                              float explosiveForceMagnitude,
                                              float explosionForceSpeed,
                                              float explosionMaxReach,
                                              bool removeCorners,
                                              float dt);
        void RemoveRows();
        bool RowContainsAsteroid(int row);
        
        Pht::IEngine& mEngine;
        Field& mField;
        FieldGravitySystem& mFieldGravity;
        ScoreManager& mScoreManager;
        EffectManager& mEffectManager;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        ExplosionsStates mExplosionsStates;
        Pht::StaticVector<int, 20> mRowsToRemove;
    };
}

#endif
