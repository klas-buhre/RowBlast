#include "MiddleIPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

MiddleIPiece::MiddleIPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Empty, Fill::Full, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0}
    };
    
    InitGrids(fillGrid, clickGrid, BlockColor::Green);
    SetPreviewCellSize(0.6f);
    SetNumRotations(2);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{1, 0}, BorderSegmentKind::Lower},
        {{1, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 0}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{1, 3}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}

Rotation MiddleIPiece::GetSpawnRotation() const {
    return Rotation::Deg90;
}
