#include "Bomb.hpp"

// Engine includes.
#include "Material.hpp"
#include "SphereMesh.hpp"
#include "IEngine.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

Bomb::Bomb(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    auto& material {scene.GetDarkGrayMaterial()};
    auto spherePieceUPtr {engine.CreateRenderableObject(Pht::SphereMesh {cellSize / 2.0f}, material)};

    auto spherePiece {spherePieceUPtr.get()};
    AddRenderable(std::move(spherePieceUPtr));

    RenderableGrid renderableGrid = {
        {nullptr, nullptr,     nullptr},
        {nullptr, spherePiece, nullptr},
        {nullptr, nullptr,     nullptr}
    };

    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty}
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
    SetNumRotations(1);
}

bool Bomb::IsBomb() const {
    return true;
}

bool Bomb::CanRotateAroundZ() const {
    return false;
}

int Bomb::GetNumEmptyTopRows() const {
    return 1;
}
