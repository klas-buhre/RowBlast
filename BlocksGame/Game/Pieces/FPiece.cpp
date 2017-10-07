#include "FPiece.hpp"

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

FPiece::FPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetRedMaterial()};

#ifdef HIGH_DETAIL
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)};
#else
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)};
#endif

    auto subPiece {subPieceUPtr.get()};
    AddRenderable(std::move(subPieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr,  nullptr,  nullptr,  nullptr},
        {nullptr,  subPiece, nullptr,  nullptr},
        {subPiece, subPiece, subPiece, subPiece},
        {nullptr,  nullptr,  nullptr,  nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty, Fill::Empty},
        {Fill::Full,  Fill::Full,  Fill::Full,  Fill::Full},
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
        {{0, 1}, BorderSegmentKind::Start},
        {{4, 1}, BorderSegmentKind::Lower},
        {{4, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::Upper},
        {{2, 2}, BorderSegmentKind::UpperRightConcaveCorner},
        {{2, 3}, BorderSegmentKind::Right},
        {{1, 3}, BorderSegmentKind::Upper},
        {{1, 2}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::UpperLeftConcaveCorner},
        {{0, 2}, BorderSegmentKind::Upper},
        {{0, 1}, BorderSegmentKind::Left},
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

int FPiece::GetNumEmptyTopRows() const {
    return 1;
}
