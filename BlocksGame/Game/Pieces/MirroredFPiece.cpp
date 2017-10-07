#include "MirroredFPiece.hpp"

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

MirroredFPiece::MirroredFPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetBlueMaterial()};

#ifdef HIGH_DETAIL
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)};
#else
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)};
#endif

    auto subPiece {subPieceUPtr.get()};
    AddRenderable(std::move(subPieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr,  nullptr,  nullptr,  nullptr},
        {subPiece, subPiece, subPiece, subPiece},
        {nullptr,  subPiece, nullptr,  nullptr},
        {nullptr,  nullptr,  nullptr,  nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Full,  Fill::Full,  Fill::Full,  Fill::Full},
        {Fill::Empty, Fill::Full,  Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};
    
    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.47f);
    
    GhostPieceBorder border {
        {{0, 2}, BorderSegmentKind::Start},
        {{1, 2}, BorderSegmentKind::Lower},
        {{1, 2}, BorderSegmentKind::LowerLeftConcaveCorner},
        {{1, 1}, BorderSegmentKind::Left},
        {{2, 1}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::LowerRightConcaveCorner},
        {{4, 2}, BorderSegmentKind::Lower},
        {{4, 3}, BorderSegmentKind::Right},
        {{0, 3}, BorderSegmentKind::Upper},
        {{0, 2}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{4, 4}};
    
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

int MirroredFPiece::GetNumEmptyTopRows() const {
    return 1;
}
