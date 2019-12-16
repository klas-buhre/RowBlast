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

        std::unique_ptr<Pht::RenderableObject> CreateRenderable(Fill fill,
                                                                BlockColor blockColor,
                                                                Pht::IEngine& engine,
                                                                const Pht::Material& material);
        void SetColor(const FallingPiece& fallingPiece);
        void RemoveFirstMovementIfDetour(const FallingPiece& fallingPiece);
        void FillWholePath(MovingPieceSnapshot movingPiece);
        void PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece, bool clearSnapshot = false);
        void ClearGrid();
        void UpdateSceneObjects();
        Pht::RenderableObject& GetRenderableObject(Fill fill, BlockColor color) const;
        int CalcRenderableIndex(Fill fill, BlockColor color) const;

        using MovementPtrs =
            Pht::StaticVector<const Movement*, Field::maxNumColumns * Field::maxNumRows * 4>;

        GameScene& mScene;
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
