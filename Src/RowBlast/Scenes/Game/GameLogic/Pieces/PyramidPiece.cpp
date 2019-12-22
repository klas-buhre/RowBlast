#include "PyramidPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

PyramidPiece::PyramidPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
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
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{2, 2}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}

bool PyramidPiece::NeedsDownAdjustmentInHud() const {
    return true;
}
