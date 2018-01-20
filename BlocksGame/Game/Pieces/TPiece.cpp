#include "TPiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "BoxMesh.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GhostPieceProducer.hpp"

using namespace BlocksGame;

TPiece::TPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetBlueMaterial()};
    auto& sceneManager {engine.GetSceneManager()};
    
#ifdef HIGH_DETAIL
    auto subPieceUPtr {
        sceneManager.CreateRenderableObject(Pht::ObjMesh {"cube_428.obj", cellSize}, material)
    };
#else
    auto subPieceUPtr {
        sceneManager.CreateRenderableObject(Pht::BoxMesh {0.95, 0.95, 0.95}, material)
    };
#endif

    auto subPiece {subPieceUPtr.get()};
    AddRenderable(std::move(subPieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr,  subPiece, nullptr},
        {nullptr,  subPiece, nullptr},
        {subPiece, subPiece, subPiece}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Empty, Fill::Full, Fill::Empty},
        {Fill::Full,  Fill::Full, Fill::Full}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1}
    };
    
    auto weldRenderable {sceneManager.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};
    
    InitGrids(renderableGrid, fillGrid, clickGrid, BlockColor::Blue, std::move(weldRenderable));
    SetPreviewCellSize(0.53f);
    
    GhostPieceBorder border {
        {{0, 0}, BorderSegmentKind::Start},
        {{3, 0}, BorderSegmentKind::Lower},
        {{3, 1}, BorderSegmentKind::Right},
        {{2, 1}, BorderSegmentKind::Upper},
        {{2, 1}, BorderSegmentKind::UpperRightConcaveCorner},
        {{2, 3}, BorderSegmentKind::Right},
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
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
