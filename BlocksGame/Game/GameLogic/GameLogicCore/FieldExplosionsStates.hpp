#ifndef FieldExplosionsStates_hpp
#define FieldExplosionsStates_hpp

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class Field;
    class ExplosionParticleEffect;
    class LaserParticleEffect;
    class FlyingBlocksAnimation;
    
    class FieldExplosionsStates {
    public:
        enum class State {
            Active,
            Inactive
        };

        FieldExplosionsStates(Pht::IEngine& engine,
                              Field& field,
                              ExplosionParticleEffect& explosionParticleEffect,
                              LaserParticleEffect& laserParticleEffect,
                              FlyingBlocksAnimation& flyingBlocksAnimation);
        
        State Update();
        void DetonateBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position);
        void DetonateLevelBomb(const Pht::IVec2& position);

    private:
        struct BombExplosionState {
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
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
                LevelBomb
            };
            
            Kind mKind;
            BombExplosionState mBombExplosionState;
            LaserState mLaserState;
        };
        
        using ExplosionsStates = Pht::StaticVector<ExplosionState, 200>;
        
        State UpdateExplosionState(ExplosionState& explosionState, float dt);
        State UpdateBombExplosionState(BombExplosionState& bombExplosionState, float dt);
        State UpdateRowBombLaserState(LaserState& laserState, float dt);
        void UpdateLeftCuttingProgress(LaserState& laserState, float dt);
        void UpdateRightCuttingProgress(LaserState& laserState, float dt);
        State UpdateLevelBombExplosionState(BombExplosionState& levelBombExplosionState, float dt);
        State UpdateGenericBombExplosionState(BombExplosionState& explosionState,
                                              float explosionForceSpeed,
                                              float explosionMaxReach,
                                              float dt);
        void RemoveRows();
        
        Pht::IEngine& mEngine;
        Field& mField;
        ExplosionParticleEffect& mExplosionParticleEffect;
        LaserParticleEffect& mLaserParticleEffect;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        ExplosionsStates mExplosionsStates;
        Pht::StaticVector<int, 10> mRowsToRemove;
    };
}

#endif
