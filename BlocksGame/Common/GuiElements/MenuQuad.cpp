#include "MenuQuad.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"

using namespace BlocksGame;

std::unique_ptr<Pht::SceneObject> MenuQuad::CreateGray(Pht::IEngine& engine,
                                                       const Pht::Vec2& size) {
    Pht::Material quadMaterial;

    Pht::QuadMesh::Vertices vertices {
        {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.5f, 0.8f}},
        {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.5f, 0.8f}},
        {{size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
        {{-size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.53f, 0.53f, 0.27, 0.8f}},
    };

    return std::make_unique<Pht::SceneObject>(
        engine.CreateRenderableObject(Pht::QuadMesh {vertices}, quadMaterial));
}

std::unique_ptr<Pht::SceneObject> MenuQuad::CreateGreen(Pht::IEngine& engine,
                                                        const Pht::Vec2& size) {
    Pht::Material quadMaterial;
    Pht::QuadMesh::Vertices vertices {
        {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.3, 0.6f, 0.3f, 0.8f}},
        {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.3, 0.6f, 0.3f, 0.8f}},
        {{size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
        {{-size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
    };

    return std::make_unique<Pht::SceneObject>(
        engine.CreateRenderableObject(Pht::QuadMesh {vertices}, quadMaterial));
}
