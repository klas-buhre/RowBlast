#include "MirroredSevenPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

MirroredSevenPiece::MirroredSevenPiece(Pht::IEngine& engine,
                                       const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Full},
        {Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Green);
    SetPreviewCellSize(0.6f);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{1, 2}, BorderSegmentKind::Upper},
        {{1, 0}, BorderSegmentKind::Left},
        {{2, 2}, BorderSegmentKind::Start},
        {{3, 2}, BorderSegmentKind::Lower},
        {{3, 3}, BorderSegmentKind::Right},
        {{2, 3}, BorderSegmentKind::Upper},
        {{2, 2}, BorderSegmentKind::Left},
        {{2, 2}, BorderSegmentKind::ConnectionForMirroredSeven}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{3, 3}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}

bool MirroredSevenPiece::NeedsLeftAdjustmentInHud() const {
    return true;
}

Rotation MirroredSevenPiece::GetSpawnRotation() const {
    return Rotation::Deg270;
}
