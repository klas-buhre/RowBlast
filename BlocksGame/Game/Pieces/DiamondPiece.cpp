#include "DiamondPiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

DiamondPiece::DiamondPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetGreenMaterial()};
    
    auto halfSubPieceUPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428.obj", cellSize}, material)
    };
    auto halfSubPiece {halfSubPieceUPtr.get()};
    AddRenderable(std::move(halfSubPieceUPtr));

    auto halfSubPiece90UPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428_r90.obj", cellSize}, material)
    };
    auto halfSubPiece90 {halfSubPiece90UPtr.get()};
    AddRenderable(std::move(halfSubPiece90UPtr));

    auto halfSubPiece180UPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428_r180.obj", cellSize}, material)
    };
    auto halfSubPiece180 {halfSubPiece180UPtr.get()};
    AddRenderable(std::move(halfSubPiece180UPtr));

    auto halfSubPiece270UPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428_r270.obj", cellSize}, material)
    };
    auto halfSubPiece270 {halfSubPiece270UPtr.get()};
    AddRenderable(std::move(halfSubPiece270UPtr));

    RenderableGrid renderableGrid = {
        {halfSubPiece,    halfSubPiece90},
        {halfSubPiece270, halfSubPiece180},
    };

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
    
    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};

    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.6f);
    SetNumRotations(1);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::LowerRightTiltForDiamond},
        {{1, 2}, BorderSegmentKind::UpperRightTiltForDiamond},
        {{0, 1}, BorderSegmentKind::UpperLeftTiltForDiamond},
        {{1, 0}, BorderSegmentKind::LowerLeftTiltForDiamond},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{2, 2}};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetFilledGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}

bool DiamondPiece::CanRotateAroundZ() const {
    return false;
}
