#include "TitleSceneRenderer.hpp"

// Engine includes.
#include "IRenderer.hpp"

// Game includes.
#include "TitleScene.hpp"
#include "RenderUtils.hpp"

using namespace BlocksGame;

TitleSceneRenderer::TitleSceneRenderer(Pht::IRenderer& engineRenderer, const TitleScene& scene) :
    mEngineRenderer {engineRenderer},
    mScene {scene} {}

void TitleSceneRenderer::RenderFrame() {
    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Perspective);
    mEngineRenderer.RenderSceneObject(mScene.GetBackground());
    RenderUtils::RenderFloatingCubes(mEngineRenderer, mScene.GetFloatingCubes());
    RenderUtils::RenderText(mEngineRenderer, mScene.GetTitleText());
    RenderUtils::RenderText(mEngineRenderer, mScene.GetTapText());
}
