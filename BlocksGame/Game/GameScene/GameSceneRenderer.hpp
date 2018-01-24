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
    class FlyingBlocksAnimation;
    class SlidingTextAnimation;
    class SubCell;
    class Piece;
    class FallingPiece;
    class PieceResources;
    class LevelResources;
    class SceneObjectPool;

    class GameSceneRenderer {
    public:
        GameSceneRenderer(Pht::IEngine& engine,
                          GameScene& scene,
                          const Field& field,
                          const GameLogic& gameLogic,
                          const FlyingBlocksAnimation& flyingBlocksAnimation,
                          const SlidingTextAnimation& slidingTextAnimation,
                          const ScrollController& scrollController,
                          const GameHud& hud,
                          const GameViewControllers& gameViewControllers,
                          const PieceResources& pieceResources,
                          const LevelResources& levelResources);
        
        void RenderFrame();
        void Render();
        
    private:
        void RenderBlueprintSlots();
        void RenderFieldBlocks();
        void RenderFieldBlock(const SubCell& subCell);
        void RenderBlockWelds(const SubCell& subCell,
                              const Pht::Vec3& blockPos,
                              Pht::RenderableObject& weldRenderalbeObject,
                              SceneObjectPool& pool);
        void RenderBlockWeld(const Pht::Vec3& weldPosition,
                             float rotation,
                             Pht::RenderableObject& weldRenderalbeObject,
                             SceneObjectPool& pool);
        void RenderFallingPiece();
        void RenderPieceBlocks(const CellGrid& pieceBlocks,
                               const Pht::Vec3& pieceFieldPos,
                               bool isTransparent,
                               SceneObjectPool& pool);
        void RenderGhostPieces();
        void RenderGhostPieceForGestureControls(const FallingPiece& fallingPiece);
        void RenderGhostPiece(Pht::RenderableObject& ghostPieceRenderable,
                              const Pht::Vec3& position,
                              Rotation rotation);
        void RenderClickableGhostPieces(const FallingPiece& fallingPiece);
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
        const FlyingBlocksAnimation& mFlyingBlocksAnimation;
        const SlidingTextAnimation& mSlidingTextAnimation;
        const ScrollController& mScrollController;
        const GameHud& mHud;
        const GameViewControllers& mGameViewControllers;
        const PieceResources& mPieceResources;
        const LevelResources& mLevelResources;
    };
}

#endif
