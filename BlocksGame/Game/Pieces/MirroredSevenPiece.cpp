#include "MirroredSevenPiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "BoxMesh.hpp"
#include "ObjMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

MirroredSevenPiece::MirroredSevenPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetGreenMaterial()};

#ifdef HIGH_DETAIL
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)};
#else
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)};
#endif

    auto subPiece {subPieceUPtr.get()};
    AddRenderable(std::move(subPieceUPtr));

    RenderableGrid renderableGrid = {
        {subPiece, nullptr,  nullptr},
        {nullptr,  subPiece, subPiece},
        {nullptr,  nullptr,  nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Full,  Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Full},
        {Fill::Empty, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0}
    };
    
    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};

    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.6f);
    
    GhostPieceBorder border {
        {{0, 2}, BorderSegmentKind::Start},
        {{1, 2}, BorderSegmentKind::Lower},
        {{1, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 2}, BorderSegmentKind::Left},
        {{1, 1}, BorderSegmentKind::Start},
        {{3, 1}, BorderSegmentKind::Lower},
        {{3, 2}, BorderSegmentKind::Right},
        {{1, 2}, BorderSegmentKind::Upper},
        {{1, 1}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::ConnectionForMirroredSeven}
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
