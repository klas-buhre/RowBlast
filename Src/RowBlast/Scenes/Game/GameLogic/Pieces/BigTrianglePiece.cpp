#include "BigTrianglePiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

BigTrianglePiece::BigTrianglePiece(Pht::IEngine& engine, const CommonResources& commonResources) {
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
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{3, 3}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}
