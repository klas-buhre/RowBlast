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
        struct ExplosionState {
            enum class Kind {
                Bomb,
                Laser,
                LevelBomb
            };
            
            Kind mKind;
            Pht::IVec2 mPosition;
            float mElapsedTime {0.0f};
        };
        
        using ExplosionsStates = Pht::StaticVector<ExplosionState, 200>;
        
        State UpdateExplosionState(ExplosionState& explosionState, float dt);
        State UpdateRowBombLaserState(ExplosionState& laserState, float dt);
        void RemoveRows();
        
        Pht::IEngine& mEngine;
        Field& mField;
        LaserParticleEffect& mLaserParticleEffect;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        ExplosionsStates mExplosionsStates;
        Pht::StaticVector<int, 10> mRowsToRemove;
    };
}

#endif
