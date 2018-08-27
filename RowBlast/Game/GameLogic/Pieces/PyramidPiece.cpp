#include "PyramidPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

PyramidPiece::PyramidPiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty,          Fill::Empty,         Fill::Empty},
        {Fill::Empty, Fill::LowerRightHalf, Fill::LowerLeftHalf, Fill::Empty},
        {Fill::Empty, Fill::Empty,          Fill::Empty,         Fill::Empty},
        {Fill::Empty, Fill::Empty,          Fill::Empty,         Fill::Empty}
    };

    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Yellow);
    SetPreviewCellSize(0.8f);
    
    GhostPieceBorder border {
        {{0, 1}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::LowerForPyramid},
        {{1, 2}, BorderSegmentKind::UpperRightTiltForPyramid},
        {{0, 1}, BorderSegmentKind::UpperLeftTiltForPyramid},
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

bool PyramidPiece::NeedsDownAdjustmentInHud() const {
    return true;
}
