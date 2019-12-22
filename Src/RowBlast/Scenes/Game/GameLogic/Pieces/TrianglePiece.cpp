#include "TrianglePiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

TrianglePiece::TrianglePiece(Pht::IEngine& engine, const CommonResources& commonResources) {
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
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{2, 2}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}
