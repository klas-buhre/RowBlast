#include "MenuQuad.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

using namespace BlocksGame;

std::unique_ptr<Pht::SceneObject> MenuQuad::CreateGray(Pht::IEngine& engine,
                                                       Pht::SceneResources& sceneResources,
                                                       const Pht::Vec2& size) {
    Pht::Material quadMaterial;

    Pht::QuadMesh::Vertices vertices {
        {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.5f, 0.8f}},
        {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.5f, 0.8f}},
        {{size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
        {{-size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.53f, 0.53f, 0.27, 0.8f}},
    };

    return engine.GetSceneManager().CreateSceneObject(Pht::QuadMesh {vertices},
                                                      quadMaterial,
                                                      sceneResources);
}

std::unique_ptr<Pht::SceneObject> MenuQuad::CreateGreen(Pht::IEngine& engine,
                                                        Pht::SceneResources& sceneResources,
                                                        const Pht::Vec2& size) {
    Pht::Material quadMaterial;
    Pht::QuadMesh::Vertices vertices {
        {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.3, 0.6f, 0.3f, 0.8f}},
        {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, {0.3, 0.6f, 0.3f, 0.8f}},
        {{size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
        {{-size.x / 2.0f, size.y / 2.0f, 0.0f}, {0.27, 0.27, 0.6f, 0.8f}},
    };

    return engine.GetSceneManager().CreateSceneObject(Pht::QuadMesh {vertices},
                                                      quadMaterial,
                                                      sceneResources);
}
