#ifndef ScenePlayingField_hpp
#define ScenePlayingField_hpp

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
    class GhostPieceBlocks;
    class LevelResources;
    class BombsAnimation;
    class AsteroidAnimation;
    class SceneObjectPool;

    class ScenePlayingField {
    public:
        ScenePlayingField(GameScene& scene,
                          const Field& field,
                          const GameLogic& gameLogic,
                          const ScrollController& scrollController,
                          const BombsAnimation& bombsAnimation,
                          AsteroidAnimation& asteroidAnimation,
                          const PieceResources& pieceResources,
                          const GhostPieceBlocks& ghostPieceBlocks,
                          const LevelResources& levelResources);
        
        void Update();
        
    private:
        void UpdateFieldGrid();
        void UpdateBlueprintSlots();
        void UpdateFieldBlocks();
        void UpdateFieldBlock(const SubCell& subCell, bool isSecondSubCell);
        void UpdateBlockWelds(const SubCell& subCell,
                              const Pht::Vec3& blockPos,
                              SceneObjectPool& pool,
                              bool isSecondSubCell);
        void UpdateBlockWeld(const Pht::Vec3& weldPosition,
                             float rotation,
                             float scale,
                             Pht::RenderableObject& weldRenderableObject,
                             SceneObjectPool& pool);
        Pht::RenderableObject& GetWeldRenderable(WeldRenderableKind renderableKind,
                                                 const SubCell& subCell,
                                                 const WeldAnimation& weldAnimation);
        void UpdateFallingPiece();
        void UpdatePieceBlocks(const CellGrid& pieceBlocks,
                               const Pht::Vec3& pieceFieldPos,
                               bool isTransparent,
                               bool isGhostPiece,
                               SceneObjectPool& pool);
        void UpdateGhostPieces();
        void UpdateGhostPieceForGestureControls(const FallingPiece& fallingPiece);
        void UpdateGhostPiece(Pht::RenderableObject& ghostPieceRenderable,
                              const Pht::Vec3& position,
                              Rotation rotation);
        void UpdateClickableGhostPieces(const FallingPiece& fallingPiece);
        void UpdateGhostPieceBlocks(const CellGrid& pieceBlocks,
                                    const Pht::Vec3& ghostPieceFieldPos);
        
        GameScene& mScene;
        const Field& mField;
        const GameLogic& mGameLogic;
        const ScrollController& mScrollController;
        const BombsAnimation& mBombsAnimation;
        AsteroidAnimation& mAsteroidAnimation;
        const PieceResources& mPieceResources;
        const GhostPieceBlocks& mGhostPieceBlocks;
        const LevelResources& mLevelResources;
    };
}

#endif