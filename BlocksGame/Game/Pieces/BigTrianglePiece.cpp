#include "BigTrianglePiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "TriangleMesh.hpp"
#include "BoxMesh.hpp"
#include "ObjMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

BigTrianglePiece::BigTrianglePiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetBlueMaterial()};
    
#ifdef HIGH_DETAIL
    auto halfSubPieceUPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"triangle_428.obj", cellSize}, material)
    };
#else
    auto halfSubPieceUPtr {engine.CreateRenderableObject(Pht::TriangleMesh {cellSize, cellSize}, material)};
#endif

    auto halfSubPiece {halfSubPieceUPtr.get()};
    AddRenderable(std::move(halfSubPieceUPtr));

#ifdef HIGH_DETAIL
    auto fullSubPieceUPtr {
        engine.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)
    };
#else
    auto fullSubPieceUPtr {engine.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)};
#endif

    auto fullSubPiece {fullSubPieceUPtr.get()};
    AddRenderable(std::move(fullSubPieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr,      nullptr,      halfSubPiece},
        {nullptr,      halfSubPiece, fullSubPiece},
        {halfSubPiece, fullSubPiece, fullSubPiece},
    };

    FillGrid fillGrid = {
        {Fill::Empty,          Fill::Empty,          Fill::LowerRightHalf},
        {Fill::Empty,          Fill::LowerRightHalf, Fill::Full},
        {Fill::LowerRightHalf, Fill::Full,           Fill::Full}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1}
    };
    
    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};

    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.6f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{3, 0}, BorderSegmentKind::LowerForTriangle},
        {{3, 3}, BorderSegmentKind::RightForTriangle},
        {{0, 0}, BorderSegmentKind::UpperLeftTiltForTriangle},
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{3, 3}};
    
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
