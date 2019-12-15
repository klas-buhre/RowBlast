#ifndef PiecePathSystem_hpp
#define PiecePathSystem_hpp

// Engine includes.
#include "StaticVector.hpp"

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
        
    private:
        struct MovingPieceSnapshot {
            Pht::IVec2 mPosition;
            Rotation mRotation;
            const Piece& mPieceType;
        };

        void RemoveFirstMovementIfDetour(const FallingPiece& fallingPiece);
        void FillWholePath(MovingPieceSnapshot movingPiece);
        void PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece, bool clearSnapshot = false);
        void ClearGrid();
        void UpdateSceneObjects();

        using MovementPtrs =
            Pht::StaticVector<const Movement*, Field::maxNumColumns * Field::maxNumRows * 4>;

        GameScene& mScene;
        std::vector<std::vector<Fill>> mPathGrid;
        int mNumRows {0};
        int mNumColumns {0};
        MovementPtrs mMovements;
        std::unique_ptr<SceneObjectPool> mSceneObjectPool;
        std::unique_ptr<Pht::RenderableObject> mRenderable;
    };
}

#endif
