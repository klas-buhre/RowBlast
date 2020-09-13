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
        void ShowPath(const FallingPiece& fallingPiece,
                      const Movement& lastMovement,
                      int lowestVisibleRow);
        void HidePath();
        void Update(float dt);
        bool IsPathVisible();
        
    private:
        struct MovingPieceSnapshot {
            Pht::IVec2 mPosition;
            Rotation mRotation;
            const Piece& mPieceType;
        };

        enum class SnapshotKind {
            Standard,
            MoveSideways,
            Last,
            Clear
        };
        
        std::unique_ptr<Pht::RenderableObject> CreateRenderable(Fill fill,
                                                                BlockColor blockColor,
                                                                Pht::IEngine& engine,
                                                                const Pht::Material& material);
        Pht::QuadMesh::Vertices CreateVertices(Fill fill, BlockColor blockColor);
        void SetColor(const FallingPiece& fallingPiece);
        void RemoveFirstMovementIfDetour(const FallingPiece& fallingPiece);
        void FillWholePath(const FallingPiece& fallingPiece);
        void PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece, SnapshotKind snapshotKind);
        void ClearSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void SetLastSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void SetSnapshotCell(int row, int column, Fill pieceSubCellFill);
        void SetSnapshotCellMovingSideways(int row, int column, Fill pieceSubCellFill);
        void ClearGrid();
        void UpdateSceneObjects();
        Pht::RenderableObject& GetRenderableObject(Fill fill, BlockColor color, int visibleRow);
        int CalcRenderableIndex(Fill fill, BlockColor color, int visibleRow) const;
        void UpdateInFadingInState(float dt);
        void UpdateInSineWaveWaitState(float dt);
        void UpdateInSineWaveState(float dt);
        void SetOpacity(float opacity, int visibleRow);
        void GoToFadingInState(const FallingPiece& fallingPiece, const Movement& lastMovement);
        void GoToSineWaveWaitState();
        void GoToSineWaveState();

        using MovementPtrs =
            Pht::StaticVector<const Movement*, Field::maxNumColumns * Field::maxNumRows * 4>;

        enum class State {
            FadingIn,
            SineWaveWait,
            SineWave,
            Inactive
        };
        
        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        int mLowestVisibleRow {0};
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
