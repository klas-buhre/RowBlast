#include "ZPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

ZPiece::ZPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Full,  Fill::Full, Fill::Empty},
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Empty, Fill::Full, Fill::Full}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Yellow);
    SetPreviewCellSize(0.53f);
    SetNumRotations(2);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{3, 0}, BorderSegmentKind::Lower},
        {{3, 1}, BorderSegmentKind::Right},
        {{2, 1}, BorderSegmentKind::Upper},
        {{2, 1}, BorderSegmentKind::UpperRightConcaveCorner},
        {{2, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 2}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::Lower},
        {{1, 2}, BorderSegmentKind::LowerLeftConcaveCorner},
        {{1, 0}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{3, 3}, commonResources};
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::No);
    
    auto renderables = ghostPieceProducer.ProduceRenderables("ZPiece");
    SetGhostPieceRenderable(std::move(renderables.mRenderable));
    SetGhostPieceShadowRenderable(std::move(renderables.mShadowRenderable));

    ghostPieceProducer.Clear();
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProducePressedRenderable());
}
