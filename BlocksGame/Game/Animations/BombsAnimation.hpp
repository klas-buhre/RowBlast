#ifndef BombsAnimation_hpp
#define BombsAnimation_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "GameHud.hpp"

namespace BlocksGame {
    class GameScene;
    class PieceResources;
    
    class BombsAnimation {
    public:
        BombsAnimation(GameScene& scene, PieceResources& pieceResources);
        
        void Init();
        void Update(float dt);
        
        const Pht::Vec3& GetBombRotation() const {
            return mBombRotation;
        }
        
        const Pht::Vec3& GetRowBombRotation() const {
            return mRowBombRotation;
        }

    private:
        void AnimateEmissive(float dt);
        void AnimateBombRotation(float dt);
        void AnimateBombPreviewPieces(ThreePreviewPieces& previewPieces);
        void AnimateRowBombRotation(float dt);
        void AnimateRowBombPreviewPieces(ThreePreviewPieces& previewPieces);
        
        GameScene& mScene;
        PieceResources& mPieceResources;
        Pht::Vec3 mBombRotation;
        Pht::Vec3 mRowBombRotation;
        float mAnimationTime {0.0f};
        float mEmissiveAnimationTime {0.0f};
    };
}

#endif
