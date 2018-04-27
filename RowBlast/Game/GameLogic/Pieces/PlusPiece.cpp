#include "PlusPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

PlusPiece::PlusPiece(Pht::IEngine& engine, const GameScene& scene) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Full,  Fill::Full, Fill::Full},
        {Fill::Empty, Fill::Full, Fill::Empty}
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
    SetPreviewCellSize(0.53f);
    SetNumRotations(1);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 1}, BorderSegmentKind::Right},
        {{2, 1}, BorderSegmentKind::LowerRightConcaveCorner},
        {{3, 1}, BorderSegmentKind::Lower},
        {{3, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::Upper},
        {{2, 2}, BorderSegmentKind::UpperRightConcaveCorner},
        {{2, 3}, BorderSegmentKind::Right},
        {{1, 3}, BorderSegmentKind::Upper},
        {{1, 2}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::UpperLeftConcaveCorner},
        {{0, 2}, BorderSegmentKind::Upper},
        {{0, 1}, BorderSegmentKind::Left},
        {{1, 1}, BorderSegmentKind::Lower},
        {{1, 1}, BorderSegmentKind::LowerLeftConcaveCorner},
        {{1, 0}, BorderSegmentKind::Left}
    };
    
    auto cellSize {scene.GetCellSize()};
    auto& commonResources {scene.GetCommonResources()};
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{3, 3}, commonResources};
    
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
