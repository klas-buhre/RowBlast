#ifndef GameSceneRenderer_hpp
#define GameSceneRenderer_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class IEngine;
    class IRenderer;
    class ParticleEffect;
    class SceneObject;
}

namespace BlocksGame {
    class Field;
    class GameLogic;
    class GameScene;
    class ScrollController;
    class GameHud;
    class GameViewControllers;
    class ExplosionParticleEffect;
    class RowExplosionParticleEffect;
    class FlyingBlocksAnimation;
    class SlidingTextAnimation;
    class BlastRadiusAnimation;
    class SubCell;
    class BlueprintCell;
    class Piece;
    class FallingPiece;

    class GameSceneRenderer {
    public:
        GameSceneRenderer(Pht::IEngine& engine,
                          GameScene& scene,
                          const Field& field,
                          const GameLogic& gameLogic,
                          const ExplosionParticleEffect& explosionParticleEffect,
                          const RowExplosionParticleEffect& rowExplosionParticleEffect,
                          const FlyingBlocksAnimation& flyingBlocksAnimation,
                          const SlidingTextAnimation& slidingTextAnimation,
                          const BlastRadiusAnimation& blastRadiusAnimation,
                          const ScrollController& scrollController,
                          const GameHud& hud,
                          const GameViewControllers& gameViewControllers);
        
        void RenderFrame();
        
    private:
        void RenderBlueprintSlots();
        void RenderFieldBlocks();
        void RenderFieldBlock(const SubCell& subCell);
        void RenderFallingPiece();
        void RenderPieceBlocks(const CellGrid& pieceBlocks,
                               const Pht::Vec3& pieceWorldPos,
                               float opacity);
        void RenderBlockWelds(const SubCell& subCell, const Pht::Vec3& blockPos, float cellSize);
        void RenderGhostPieces();
        void RenderGhostPiece(const FallingPiece& fallingPiece,
                              const Pht::Vec3& ghostPieceCenterLocalCoords);
        void RenderClickableGhostPieces(const FallingPiece& fallingPiece,
                                        const Pht::Vec3& ghostPieceCenterLocalCoords);
        void RenderGhostPiece(const Pht::RenderableObject& ghostPieceRenderable,
                              const Pht::Vec3& position,
                              Rotation rotation);
        void RenderBlastRadiusAnimation();
        void RenderExplosion();
        void RenderRowExplosion();
        void RenderFlyingBlocks();
        void RenderHud();
        void RenderTiltedGrayBlockInHud(const Pht::RenderableObject& grayBlock);
        void RenderTiltedBlueprintSlotInHud();
        void RenderTiltedLPieceInHud();
        void RenderPreviewPiece(const Piece* piece, const Pht::Vec2& position);
        void RenderScaledTiltedPiece(const Pht::Vec2& position,
                                     float cellSize,
                                     int pieceNumRows,
                                     int pieceNumColumns,
                                     const CellGrid& pieceGrid);
        void RenderGameViews();
        void RenderLevelCompletedView();
        void RenderSlidingText();
        
        Pht::IEngine& mEngine;
        Pht::IRenderer& mEngineRenderer;
        GameScene& mScene;
        const Field& mField;
        const GameLogic& mGameLogic;
        const ExplosionParticleEffect& mExplosionParticleEffect;
        const RowExplosionParticleEffect& mRowExplosionParticleEffect;
        const FlyingBlocksAnimation& mFlyingBlocksAnimation;
        const SlidingTextAnimation& mSlidingTextAnimation;
        const BlastRadiusAnimation& mBlastRadiusAnimation;
        const ScrollController& mScrollController;
        const GameHud& mHud;
        const GameViewControllers& mGameViewControllers;
    };
}

#endif
