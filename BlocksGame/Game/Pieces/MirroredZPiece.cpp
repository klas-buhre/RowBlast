#include "MirroredZPiece.hpp"

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

MirroredZPiece::MirroredZPiece(Pht::IEngine& engine, const GameScene& scene) {
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
        {nullptr,  subPiece, subPiece},
        {nullptr,  subPiece, nullptr},
        {subPiece, subPiece, nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Full, Fill::Full},
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Full,  Fill::Full, Fill::Empty}
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
    SetPreviewCellSize(0.53f);
    SetNumRotations(2);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::LowerRightConcaveCorner},
        {{3, 2}, BorderSegmentKind::Lower},
        {{3, 3}, BorderSegmentKind::Right},
        {{1, 3}, BorderSegmentKind::Upper},
        {{1, 1}, BorderSegmentKind::Left},
        {{1, 1}, BorderSegmentKind::UpperLeftConcaveCorner},
        {{0, 1}, BorderSegmentKind::Upper},
        {{0, 0}, BorderSegmentKind::Left}
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
