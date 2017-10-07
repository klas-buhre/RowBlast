#include "MiddleIPiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "BoxMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

MiddleIPiece::MiddleIPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetYellowMaterial()};
    
#ifdef HIGH_DETAIL
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)};
#else
    auto subPieceUPtr {engine.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)};
#endif

    auto subPiece {subPieceUPtr.get()};
    AddRenderable(std::move(subPieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr,  nullptr,  nullptr},
        {subPiece, subPiece, subPiece},
        {nullptr,  nullptr,  nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Full,  Fill::Full,  Fill::Full},
        {Fill::Empty, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0}
    };
    
    auto weldRenderable {engine.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};
    
    InitGrids(renderableGrid, fillGrid, clickGrid, std::move(weldRenderable));
    SetPreviewCellSize(0.6f);
    SetNumRotations(2);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{3, 0}, BorderSegmentKind::Lower},
        {{3, 1}, BorderSegmentKind::Right},
        {{0, 1}, BorderSegmentKind::Upper},
        {{0, 0}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{3, 1}};
    
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

int MiddleIPiece::GetNumEmptyTopRows() const {
    return 1;
}
