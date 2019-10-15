#include "LongIPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

LongIPiece::LongIPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Full, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Full, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Yellow);
    SetPreviewCellSize(0.4f);
    SetNumRotations(2);

    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{1, 0}, BorderSegmentKind::Lower},
        {{1, 5}, BorderSegmentKind::Right},
        {{0, 5}, BorderSegmentKind::Upper},
        {{0, 0}, BorderSegmentKind::Left}
    };

    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{1, 5}, commonResources};
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::No);
    
    auto renderables = ghostPieceProducer.ProduceRenderables("LongIPiece");
    SetGhostPieceRenderable(std::move(renderables.mRenderable));
    SetGhostPieceShadowRenderable(std::move(renderables.mShadowRenderable));

    ghostPieceProducer.Clear();
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProducePressedRenderable());
}

Rotation LongIPiece::GetSpawnRotation() const {
    return Rotation::Deg90;
}
