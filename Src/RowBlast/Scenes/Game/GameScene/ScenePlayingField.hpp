#ifndef FieldSceneSystem_hpp
#define FieldSceneSystem_hpp

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
    class FallingPieceAnimation;
    class DraggedPieceAnimation;
    class SceneObjectPool;
    class Piece;

    class FieldSceneSystem {
    public:
        FieldSceneSystem(GameScene& scene,
                         const Field& field,
                         const GameLogic& gameLogic,
                         const ScrollController& scrollController,
                         const BombsAnimation& bombsAnimation,
                         AsteroidAnimation& asteroidAnimation,
                         const FallingPieceAnimation& fallingPieceAnimation,
                         const DraggedPieceAnimation& draggedPieceAnimation,
                         const PieceResources& pieceResources,
                         const GhostPieceBlocks& ghostPieceBlocks,
                         const LevelResources& levelResources);
        
        void Update();
        
    private:
        void UpdateFieldGrid();
        void UpdateBlueprintSlots();
        void UpdateFieldBlocks();
        void UpdateFieldBlock(const SubCell& subCell, bool isSecondSubCell);
        void UpdateBlockBonds(const SubCell& subCell,
                              const Pht::Vec3& blockPos,
                              SceneObjectPool& pool,
                              bool isSecondSubCell);
        Pht::RenderableObject& GetBondRenderable(BondRenderableKind renderableKind,
                                                 const SubCell& subCell,
                                                 const BondAnimation& bondAnimation);
        void UpdateFallingPiece();
        Pht::Vec2 CalculateFallingPieceGridPosition(const FallingPiece& fallingPiece);
        void UpdateDraggedPiece();
        void UpdatePieceBlocks(const Piece& pieceType,
                               Rotation rotation,
                               const Pht::Vec3& pieceFieldPos,
                               bool isTransparent,
                               bool isGhostPiece,
                               SceneObjectPool& pool);
        void UpdateGhostPieces();
        void UpdateGhostPieceForGestureControls(const Piece& pieceType,
                                                int column,
                                                int ghostPieceRow,
                                                Rotation rotation);
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
        const FallingPieceAnimation& mFallingPieceAnimation;
        const DraggedPieceAnimation& mDraggedPieceAnimation;
        const PieceResources& mPieceResources;
        const GhostPieceBlocks& mGhostPieceBlocks;
        const LevelResources& mLevelResources;
    };
}

#endif
