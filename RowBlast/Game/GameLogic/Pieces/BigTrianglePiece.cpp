#include "BigTrianglePiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

BigTrianglePiece::BigTrianglePiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::Empty,          Fill::Empty,          Fill::LowerRightHalf},
        {Fill::Empty,          Fill::LowerRightHalf, Fill::Full},
        {Fill::LowerRightHalf, Fill::Full,           Fill::Full}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Blue);
    SetPreviewCellSize(0.6f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{3, 0}, BorderSegmentKind::LowerForTriangle},
        {{3, 3}, BorderSegmentKind::RightForTriangle},
        {{0, 0}, BorderSegmentKind::UpperLeftTiltForTriangle},
    };
    
    auto cellSize {scene.GetCellSize()};
    auto& commonResources {scene.GetCommonResources()};
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{3, 3}, commonResources};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());

    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorder();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
