#ifndef GameSceneRenderer_hpp
#define GameSceneRenderer_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class RenderableObject;
}

namespace BlocksGame {
    class Field;
    class GameLogic;
    class GameScene;
    class ScrollController;
    class SubCell;
    class FallingPiece;
    class PieceResources;
    class LevelResources;
    class SceneObjectPool;

    class GameSceneRenderer {
    public:
        GameSceneRenderer(GameScene& scene,
                          const Field& field,
                          const GameLogic& gameLogic,
                          const ScrollController& scrollController,
                          const PieceResources& pieceResources,
                          const LevelResources& levelResources);
        
        void Render();
        
    private:
        void RenderBlueprintSlots();
        void RenderFieldBlocks();
        void RenderFieldBlock(const SubCell& subCell, bool isSecondSubCell);
        void RenderBlockWelds(const SubCell& subCell,
                              const Pht::Vec3& blockPos,
                              Pht::RenderableObject& weldRenderalbeObject,
                              SceneObjectPool& pool,
                              bool isSecondSubCell);
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
        
        GameScene& mScene;
        const Field& mField;
        const GameLogic& mGameLogic;
        const ScrollController& mScrollController;
        const PieceResources& mPieceResources;
        const LevelResources& mLevelResources;
    };
}

#endif
