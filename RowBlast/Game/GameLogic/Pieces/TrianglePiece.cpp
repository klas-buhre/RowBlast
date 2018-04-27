#include "TrianglePiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

TrianglePiece::TrianglePiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::Empty,          Fill::LowerRightHalf},
        {Fill::LowerRightHalf, Fill::Full}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Red);
    SetPreviewCellSize(0.6f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::LowerForTriangle},
        {{2, 2}, BorderSegmentKind::RightForTriangle},
        {{0, 0}, BorderSegmentKind::UpperLeftTiltForTriangle},
    };
    
    auto cellSize {scene.GetCellSize()};
    auto& commonResources {scene.GetCommonResources()};
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{2, 2}, commonResources};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetFilledGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());

    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
