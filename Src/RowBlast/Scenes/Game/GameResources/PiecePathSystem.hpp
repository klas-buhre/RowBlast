#ifndef PiecePathSystem_hpp
#define PiecePathSystem_hpp

// Engine includes.
#include "StaticVector.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "Field.hpp"
#include "SceneObjectPool.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class Movement;
    class Level;
    class FallingPiece;

    class PiecePathSystem {
    public:
        PiecePathSystem(Pht::IEngine& engine, GameScene& scene);
        
        void Init(const Level& level);
        void ShowPath(const FallingPiece& fallingPiece, const Movement& lastMovement);
        void HidePath();
        bool IsPathVisible();
        
    private:
        struct MovingPieceSnapshot {
            Pht::IVec2 mPosition;
            Rotation mRotation;
            const Piece& mPieceType;
        };

        std::unique_ptr<Pht::RenderableObject> CreateRenderable(Fill fill,
                                                                BlockColor blockColor,
                                                                Pht::IEngine& engine,
                                                                const Pht::Material& material);
        Pht::QuadMesh::Vertices CreateVertices(Fill fill, BlockColor blockColor);
        void SetColor(const FallingPiece& fallingPiece);
        void RemoveFirstMovementIfDetour(const FallingPiece& fallingPiece);
        void FillWholePath(MovingPieceSnapshot movingPiece);
        void PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece,
                                bool lastSnapshot,
                                bool clearSnapshot = false);
        void ClearSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void SetLastSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void SetSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void ClearGrid();
        void UpdateSceneObjects();
        Pht::RenderableObject& GetRenderableObject(Fill fill, BlockColor color) const;
        int CalcRenderableIndex(Fill fill, BlockColor color) const;

        using MovementPtrs =
            Pht::StaticVector<const Movement*, Field::maxNumColumns * Field::maxNumRows * 4>;

        enum class State {
            Active,
            Inactive
        };
        
        GameScene& mScene;
        State mState {State::Inactive};
        std::vector<std::vector<Fill>> mPathGrid;
        int mNumRows {0};
        int mNumColumns {0};
        MovementPtrs mMovements;
        BlockColor mColor {BlockColor::Red};
        std::unique_ptr<SceneObjectPool> mSceneObjectPool;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mRenderables;
    };
}

#endif
