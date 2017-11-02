#include "PlusPiece.hpp"

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

PlusPiece::PlusPiece(Pht::IEngine& engine, const GameScene& scene) {
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
        {nullptr,  subPiece, nullptr},
        {subPiece, subPiece, subPiece},
        {nullptr,  subPiece, nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Full,  Fill::Full, Fill::Full},
        {Fill::Empty, Fill::Full, Fill::Empty}
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
    SetNumRotations(1);
    
    GhostPieceBorder border {
        {{1, 0}, BorderSegmentKind::Start},
        {{2, 0}, BorderSegmentKind::Lower},
        {{2, 1}, BorderSegmentKind::Right},
        {{2, 1}, BorderSegmentKind::LowerRightConcaveCorner},
        {{3, 1}, BorderSegmentKind::Lower},
        {{3, 2}, BorderSegmentKind::Right},
        {{2, 2}, BorderSegmentKind::Upper},
        {{2, 2}, BorderSegmentKind::UpperRightConcaveCorner},
        {{2, 3}, BorderSegmentKind::Right},
        {{1, 3}, BorderSegmentKind::Upper},
        {{1, 2}, BorderSegmentKind::Left},
        {{1, 2}, BorderSegmentKind::UpperLeftConcaveCorner},
        {{0, 2}, BorderSegmentKind::Upper},
        {{0, 1}, BorderSegmentKind::Left},
        {{1, 1}, BorderSegmentKind::Lower},
        {{1, 1}, BorderSegmentKind::LowerLeftConcaveCorner},
        {{1, 0}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{3, 3}};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
