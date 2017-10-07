#include "SpherePiece.hpp"

// Engine includes.
#include "Material.hpp"
#include "SphereMesh.hpp"
#include "IEngine.hpp"
#include "BoxMesh.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

SpherePiece::SpherePiece(Pht::IEngine& engine, const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    Pht::Material material {"smiley.jpg", 0.45f, 0.75f, 1.0f, 50};
    
    auto spherePieceUPtr {
        engine.CreateRenderableObject(
            Pht::SphereMesh {
                cellSize,
                Pht::Vec2{1.0f, 2.0f},
                Pht::Vec3{cellSize / 2.0f, -cellSize / 2.0f, 0.0f}
            },
            material)
    };

    auto spherePiece {spherePieceUPtr.get()};
    AddRenderable(std::move(spherePieceUPtr));

    RenderableGrid renderableGrid = {
        {spherePiece, nullptr},
        {nullptr,     nullptr}
    };

    FillGrid fillGrid = {
        {Fill::LowerRightHalf, Fill::LowerLeftHalf},
        {Fill::UpperRightHalf, Fill::UpperLeftHalf}
    };
    
    ClickGrid clickGrid = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };

    InitGrids(renderableGrid, fillGrid, clickGrid, nullptr, true);
    SetPreviewCellSize(1.5f);
    SetNumRotations(1);
}

bool SpherePiece::CanRotateAroundZ() const {
    return false;
}
