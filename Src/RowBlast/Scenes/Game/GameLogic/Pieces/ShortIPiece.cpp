#include "ShortIPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

ShortIPiece::ShortIPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty}
    };

    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    
    InitGrids(fillGrid, clickGrid, BlockColor::Blue);
    SetPreviewCellSize(0.6f);

    SetDuplicateMoveCheck(Rotation::Deg270, {{0, -1}, Rotation::Deg90});
    SetDuplicateMoveCheck(Rotation::Deg0, {{1, 0}, Rotation::Deg180});
    SetDuplicateMoveCheck(Rotation::Deg90, {{0, -1}, Rotation::Deg270});
    SetDuplicateMoveCheck(Rotation::Deg180, {{1, 0}, Rotation::Deg0});

    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{1, 2}, BorderSegmentKind::Upper},
        {{1, 0}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{2, 2}, commonResources};
    auto renderables = ghostPieceProducer.DrawRenderables(border, GetColor());
    SetDraggedPieceRenderable(std::move(renderables.mDraggedPiece));
    SetHighlightedDraggedPieceRenderable(std::move(renderables.mHighlightedDraggedPiece));
    SetShadowRenderable(std::move(renderables.mShadow));
    SetGhostPieceRenderable(std::move(renderables.mGhostPiece));
    SetHighlightedGhostPieceRenderable(std::move(renderables.mHighlightedGhostPiece));
}

bool ShortIPiece::NeedsLeftAdjustmentInHud() const {
    return true;
}

Rotation ShortIPiece::GetSpawnRotation() const {
    return Rotation::Deg270;
}
