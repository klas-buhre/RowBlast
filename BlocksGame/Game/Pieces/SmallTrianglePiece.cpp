#include "SmallTrianglePiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "TriangleMesh.hpp"
#include "IEngine.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

SmallTrianglePiece::SmallTrianglePiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetRedMaterial()};
    
#ifdef HIGH_DETAIL
    auto halfSubPieceUPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428.obj", cellSize}, material)
    };
#else
    auto halfSubPieceUPtr {engine.CreateRenderableObject(Pht::TriangleMesh {cellSize, cellSize}, material)};
#endif
    
    auto halfSubPiece {halfSubPieceUPtr.get()};
    AddRenderable(std::move(halfSubPieceUPtr));
    
    RenderableGrid renderableGrid = {
        {nullptr, nullptr,      nullptr},
        {nullptr, halfSubPiece, nullptr},
        {nullptr, nullptr,      nullptr}
    };

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

    InitGrids(renderableGrid, fillGrid, clickGrid, nullptr);
    SetPreviewCellSize(0.8f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{1, 0}, BorderSegmentKind::LowerForTriangle},
        {{1, 1}, BorderSegmentKind::RightForTriangle},
        {{0, 0}, BorderSegmentKind::UpperLeftTiltForTriangle},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{1, 1}};
    
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
