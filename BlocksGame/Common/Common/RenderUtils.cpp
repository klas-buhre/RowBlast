#include "RenderUtils.hpp"

// Engine includes.
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"

// Game includes.
#include "GradientRectangle.hpp"

using namespace BlocksGame;

void RenderUtils::RenderText(Pht::IRenderer& renderer, const Pht::Text& text) {
    renderer.RenderText(text.mText, text.mPosition, text.mProperties);
}

void RenderUtils::RenderGradientRectangle(Pht::IRenderer& renderer,
                                          const GradientRectangle& rectangle) {
    renderer.RenderSceneObject(rectangle.GetMidQuad());
    renderer.RenderSceneObject(rectangle.GetLeftQuad());
    renderer.RenderSceneObject(rectangle.GetRightQuad());
}
