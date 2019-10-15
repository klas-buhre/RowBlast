#include "SevenPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

SevenPiece::SevenPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::Full,  Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 0, 0}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Blue);
    SetPreviewCellSize(0.6f);

    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{1, 2}, BorderSegmentKind::Upper},
        {{1, 0}, BorderSegmentKind::Left},
        {{0, 2}, BorderSegmentKind::Start},
        {{1, 2}, BorderSegmentKind::Lower},
        {{1, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 2}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::ConnectionForSeven}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{3, 3}, commonResources};
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::No);

    auto renderables = ghostPieceProducer.ProduceRenderables("SevenPiece");
    SetGhostPieceRenderable(std::move(renderables.mRenderable));
    SetGhostPieceShadowRenderable(std::move(renderables.mShadowRenderable));

    ghostPieceProducer.Clear();
    ghostPieceProducer.DrawBorder(border, GetColor(), PressedGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProducePressedRenderable());
}

int SevenPiece::GetSpawnYPositionAdjustment() const {
    return -1;
}

bool SevenPiece::NeedsRightAdjustmentInHud() const {
    return true;
}

Rotation SevenPiece::GetSpawnRotation() const {
    return Rotation::Deg270;
}
