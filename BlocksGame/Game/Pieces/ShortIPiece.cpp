#include "ShortIPiece.hpp"

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

ShortIPiece::ShortIPiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetGreenMaterial()};
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
        {nullptr, nullptr,   nullptr, nullptr},
        {nullptr, subPiece, subPiece, nullptr},
        {nullptr, nullptr,   nullptr, nullptr},
        {nullptr, nullptr,   nullptr, nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty, Fill::Empty}
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
    
    auto weldRenderable {sceneManager.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)};
    
    InitGrids(renderableGrid, fillGrid, clickGrid, BlockColor::Green, std::move(weldRenderable));
    SetPreviewCellSize(0.6f);
    SetNumRotations(2);
    
    GhostPieceBorder border {
        {{0, 1}, BorderSegmentKind::Start},
        {{2, 1}, BorderSegmentKind::Lower},
        {{2, 2}, BorderSegmentKind::Right},
        {{0, 2}, BorderSegmentKind::Upper},
        {{0, 1}, BorderSegmentKind::Left}
    };
    
    GhostPieceProducer ghostPieceProducer {engine, cellSize, Pht::IVec2{2, 2}};
    
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::No);
    SetGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
    
    ghostPieceProducer.Clear();
    ghostPieceProducer.SetBrightBorderAndFill();
    ghostPieceProducer.DrawBorder(border, FillGhostPiece::Yes);
    SetPressedGhostPieceRenderable(ghostPieceProducer.ProduceRenderable());
}
