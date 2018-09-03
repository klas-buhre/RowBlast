#ifndef GameSceneRenderer_hpp
#define GameSceneRenderer_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"
#include "PieceResources.hpp"

namespace Pht {
    class RenderableObject;
}

namespace RowBlast {
    class Field;
    class GameLogic;
    class GameScene;
    class ScrollController;
    class SubCell;
    class FallingPiece;
    class PieceResources;
    class LevelResources;
    class BombsAnimation;
    class SceneObjectPool;

    class GameSceneRenderer {
    public:
        GameSceneRenderer(GameScene& scene,
                          const Field& field,
                          const GameLogic& gameLogic,
                          const ScrollController& scrollController,
                          const BombsAnimation& bombsAnimation,
                          const PieceResources& pieceResources,
                          const LevelResources& levelResources);
        
        void Render();
        
    private:
        void RenderBlueprintSlots();
        void RenderFieldBlocks();
        void RenderFieldBlock(const SubCell& subCell, bool isSecondSubCell);
        void RenderBlockWelds(const SubCell& subCell,
                              const Pht::Vec3& blockPos,
                              SceneObjectPool& pool,
                              bool isSecondSubCell);
        void RenderBlockWeld(const Pht::Vec3& weldPosition,
                             float rotation,
                             float scale,
                             Pht::RenderableObject& weldRenderableObject,
                             SceneObjectPool& pool);
        Pht::RenderableObject& GetWeldRenderable(WeldRenderableKind renderableKind,
                                                 const SubCell& subCell,
                                                 const WeldAnimation& weldAnimation);
        void RenderFallingPiece();
        void RenderPieceBlocks(const CellGrid& pieceBlocks,
                               const Pht::Vec3& pieceFieldPos,
                               bool isTransparent,
                               bool isGhostPiece,
                               SceneObjectPool& pool);
        void RenderGhostPieces();
        void RenderGhostPieceForGestureControls(const FallingPiece& fallingPiece);
        void RenderGhostPiece(Pht::RenderableObject& ghostPieceRenderable,
                              const Pht::Vec3& position,
                              Rotation rotation);
        void RenderClickableGhostPieces(const FallingPiece& fallingPiece);
        
        GameScene& mScene;
        const Field& mField;
        const GameLogic& mGameLogic;
        const ScrollController& mScrollController;
        const BombsAnimation& mBombsAnimation;
        const PieceResources& mPieceResources;
        const LevelResources& mLevelResources;
    };
}

#endif
