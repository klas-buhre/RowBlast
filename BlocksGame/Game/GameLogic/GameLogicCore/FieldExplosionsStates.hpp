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
                              LaserParticleEffect& laserParticleEffect,
                              FlyingBlocksAnimation& flyingBlocksAnimation);
        
        State Update();
        void DetonateBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateRowBomb(const Pht::IVec2& position, const Pht::Vec2& exactPosition);
        void DetonateLevelBomb(const Pht::IVec2& position);

    private:
        // Maybe these structs can be changed into one single struct:
        // ExplosionState {mKind, mPosition, mElapsedTime}
        
        struct BombExplosionState {
            enum class State {
                Stage1,
                Stage2,
            };
            
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
            State mState {State::Stage1};
        };

        struct RowBombLaserState {
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
        };

        struct LevelBombExplosionState {
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
        };
        
        using BombExplosionsStates = Pht::StaticVector<BombExplosionState, 10>;
        using RowBombLasersStates = Pht::StaticVector<RowBombLaserState, 10>;
        using LevelBombExplosionsStates = Pht::StaticVector<LevelBombExplosionState, 100>;
        
        State UpdateRowBombLaserState(RowBombLaserState& laserState, float dt);
        void RemoveRows();
        
        Pht::IEngine& mEngine;
        Field& mField;
        LaserParticleEffect& mLaserParticleEffect;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        BombExplosionsStates mBombExplosionsStates;
        RowBombLasersStates mRowBombLasersStates;
        LevelBombExplosionsStates mLevelBombExplosionsStates;
        Pht::StaticVector<int, 10> mRowsToRemove;
    };
}

#endif
