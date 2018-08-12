#ifndef BombsAnimation_hpp
#define BombsAnimation_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "GameHud.hpp"

namespace RowBlast {
    class GameScene;
    class PieceResources;
    class LevelResources;
    
    class BombsAnimation {
    public:
        BombsAnimation(GameScene& scene,
                       PieceResources& pieceResources,
                       LevelResources& levelResources);
        
        void Init();
        void Update(float dt);
        
        const Pht::Vec3& GetBombRotation() const {
            return mBombRotation;
        }
        
        const Pht::Vec3& GetRowBombRotation() const {
            return mRowBombRotation;
        }
        
        const Pht::Vec3& GetBombStaticRotation() const {
            return mBombStaticRotation;
        }
        
        const Pht::Vec3& GetRowBombStaticRotation() const {
            return mRowBombStaticRotation;
        }

    private:
        void AnimateEmissive(float dt);
        void AnimateBombRotation(float dt);
        void AnimateRowBombRotation(float dt);
        
        GameScene& mScene;
        PieceResources& mPieceResources;
        LevelResources& mLevelResources;
        Pht::Vec3 mBombRotation;
        Pht::Vec3 mRowBombRotation;
        float mAnimationTime {0.0f};
        float mEmissiveAnimationTime {0.0f};
        const Pht::Vec3 mBombStaticRotation {110.0f, -20.0f, 0.0f};
        const Pht::Vec3 mRowBombStaticRotation {0.0f, 90.0f, 0.0f};
    };
}

#endif
