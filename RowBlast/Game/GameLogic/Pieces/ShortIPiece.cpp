#include "ShortIPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

ShortIPiece::ShortIPiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty}
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
    
    InitGrids(fillGrid, clickGrid, BlockColor::Blue);
    SetPreviewCellSize(0.6f);
    
    SetDuplicateMoveCheck(Rotation::Deg0, {{0, 1}, Rotation::Deg180});
    SetDuplicateMoveCheck(Rotation::Deg90, {{1, 0}, Rotation::Deg270});
    SetDuplicateMoveCheck(Rotation::Deg180, {{0, -1}, Rotation::Deg0});
    SetDuplicateMoveCheck(Rotation::Deg270, {{-1, 0}, Rotation::Deg90});

    GhostPieceBorder border {
        {{0, 1}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{0, 2}, BorderSegmentKind::Upper},
        {{0, 1}, BorderSegmentKind::Left}
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

bool ShortIPiece::NeedsDownAdjustmentInHud() const {
    return true;
}
