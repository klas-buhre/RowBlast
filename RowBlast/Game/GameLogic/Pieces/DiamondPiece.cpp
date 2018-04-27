#include "DiamondPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

DiamondPiece::DiamondPiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::LowerRightHalf, Fill::LowerLeftHalf},
        {Fill::UpperRightHalf, Fill::UpperLeftHalf}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Green);
    SetPreviewCellSize(0.6f);
    SetNumRotations(1);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::LowerRightTiltForDiamond},
        {{1, 2}, BorderSegmentKind::UpperRightTiltForDiamond},
        {{0, 1}, BorderSegmentKind::UpperLeftTiltForDiamond},
        {{1, 0}, BorderSegmentKind::LowerLeftTiltForDiamond},
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

bool DiamondPiece::CanRotateAroundZ() const {
    return false;
}
