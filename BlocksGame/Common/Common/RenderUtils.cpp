#include "RenderUtils.hpp"

// Engine includes.
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "GradientRectangle.hpp"

using namespace BlocksGame;

void RenderUtils::RenderParticleEffect(Pht::IRenderer& renderer,
                                       const Pht::ParticleEffect_& effect) {
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
    renderer.RenderSceneObject(rectangle.GetMidQuad());
    renderer.RenderSceneObject(rectangle.GetLeftQuad());
    renderer.RenderSceneObject(rectangle.GetRightQuad());
}
