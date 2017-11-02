#include "PyramidPiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

PyramidPiece::PyramidPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetYellowMaterial()};
    
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

    RenderableGrid renderableGrid = {
        {nullptr, nullptr,      nullptr,        nullptr},
        {nullptr, halfSubPiece, halfSubPiece90, nullptr},
        {nullptr, nullptr,      nullptr,        nullptr},
        {nullptr, nullptr,      nullptr,        nullptr}
    };

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
    
    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};

    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.8f);
    
    GhostPieceBorder border {
        {{0, 1}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::LowerForPyramid},
        {{1, 2}, BorderSegmentKind::UpperRightTiltForPyramid},
        {{0, 1}, BorderSegmentKind::UpperLeftTiltForPyramid},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{2, 2}};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
