#include "MirroredFPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

MirroredFPiece::MirroredFPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Full,  Fill::Full,  Fill::Full,  Fill::Full},
        {Fill::Empty, Fill::Full,  Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    
    InitGrids(fillGrid, clickGrid, BlockColor::Blue);
    SetPreviewCellSize(0.47f);
    
    GhostPieceBorder border {
        {{0, 2}, BorderSegmentKind::Start},
        {{1, 2}, BorderSegmentKind::Lower},
        {{1, 2}, BorderSegmentKind::LowerLeftConcaveCorner},
        {{1, 1}, BorderSegmentKind::Left},
        {{2, 1}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::LowerRightConcaveCorner},
        {{4, 2}, BorderSegmentKind::Lower},
        {{4, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 2}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{4, 4}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}
