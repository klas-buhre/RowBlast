#include "SmallTrianglePiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

SmallTrianglePiece::SmallTrianglePiece(Pht::IEngine& engine,
                                       const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty,          Fill::Empty},
        {Fill::Empty, Fill::LowerRightHalf, Fill::Empty},
        {Fill::Empty, Fill::Empty,          Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Red);
    SetPreviewCellSize(0.8f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{1, 0}, BorderSegmentKind::LowerForTriangle},
        {{1, 1}, BorderSegmentKind::RightForTriangle},
        {{0, 0}, BorderSegmentKind::UpperLeftTiltForTriangle},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{1, 1}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}

bool SmallTrianglePiece::PositionCanBeAdjusteInMovesSearch() const {
    return false;
}
