#include "RenderUtils.hpp"

// Engine includes.
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "ParticleSystem.hpp"

// Game includes.
#include "GradientRectangle.hpp"

using namespace BlocksGame;

void RenderUtils::RenderSceneObject(Pht::IRenderer& renderer,
                                    const Pht::SceneObject& sceneObject) {
    auto* renderable {sceneObject.GetRenderable()};
    
    if (renderable) {
        renderer.Render(*renderable, sceneObject.GetMatrix());
    }
}

void RenderUtils::RenderParticleEffect(Pht::IRenderer& renderer,
                                       const Pht::ParticleEffect& effect) {
    auto* renderable {effect.mParticleSystem->GetRenderableObject()};
    
    if (renderable) {
        renderer.Render(*renderable, effect.mTransform);
    }
}

void RenderUtils::RenderText(Pht::IRenderer& renderer, const Pht::Text& text) {
    renderer.RenderText(text.mText, text.mPosition, text.mProperties);
}

void RenderUtils::RenderFloatingCubes(Pht::IRenderer& renderer,
                                      const std::vector<FloatingCube>& cubes) {
    for (const auto& cube: cubes) {
        auto rotationMatrix {Pht::Mat4::RotateX(cube.mOrientation.x) *
                             Pht::Mat4::RotateY(cube.mOrientation.y) *
                             Pht::Mat4::RotateZ(cube.mOrientation.z)};
        auto matrix {rotationMatrix * Pht::Mat4::Translate(cube.mPosition.x,
                                                            cube.mPosition.y,
                                                            cube.mPosition.z)};
        renderer.Render(*cube.mRenderable, matrix);
    }
}

void RenderUtils::RenderGradientRectangle(Pht::IRenderer& renderer,
                                          const GradientRectangle& rectangle) {
    RenderSceneObject(renderer, rectangle.GetMidQuad());
    RenderSceneObject(renderer, rectangle.GetLeftQuad());
    RenderSceneObject(renderer, rectangle.GetRightQuad());
}
