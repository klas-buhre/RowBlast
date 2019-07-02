#include "DiamondPiece.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "GhostPieceProducer.hpp"

using namespace RowBlast;

DiamondPiece::DiamondPiece(Pht::IEngine& engine, const CommonResources& commonResources) {
    FillGrid fillGrid = {
        {Fill::LowerRightHalf, Fill::LowerLeftHalf},
        {Fill::UpperRightHalf, Fill::UpperLeftHalf}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::Green);
    SetPreviewCellSize(0.6f);
    SetNumRotations(1);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::LowerRightTiltForDiamond},
        {{1, 2}, BorderSegmentKind::UpperRightTiltForDiamond},
        {{0, 1}, BorderSegmentKind::UpperLeftTiltForDiamond},
        {{1, 0}, BorderSegmentKind::LowerLeftTiltForDiamond},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, Pht::IVec2{2, 2}, commonResources};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());

    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorder();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
