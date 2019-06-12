#include "TutorialFieldBottomGlowAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "TutorialUiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto glowWidthInCells = 7.0f;
    constexpr auto glowHeightInCells = 1.0f;
    constexpr auto glowOpacity = 0.275f;
    constexpr auto pulseAmplitude = 0.435f;
    constexpr auto pulseDuration = 1.25f;
}

TutorialFieldBottomGlowAnimation::TutorialFieldBottomGlowAnimation(Pht::IEngine& engine,
                                                                   Pht::SceneObject& parent) {
    Pht::Material glowMaterial;
    glowMaterial.SetBlend(Pht::Blend::Additive);
    glowMaterial.SetDepthTest(false);
    glowMaterial.SetOpacity(glowOpacity);
    
    auto& sceneManager = engine.GetSceneManager();
    auto cellSize = 1.0f;
    auto halfWidth = cellSize * glowWidthInCells / 2.0f;
    auto halfHeight = cellSize * glowHeightInCells / 2.0f;
    auto f = 0.9f;

    Pht::QuadMesh::Vertices vertices {
        {{-halfWidth, -halfHeight, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{halfWidth, -halfHeight, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{halfWidth, halfHeight, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 0.0f}},
        {{-halfWidth, halfHeight, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 0.0f}}
    };

    mGlowRenderable = sceneManager.CreateRenderableObject(Pht::QuadMesh {vertices}, glowMaterial);
    mGlowSceneObject = std::make_unique<Pht::SceneObject>(mGlowRenderable.get());
    mGlowSceneObject->GetTransform().SetPosition({0.0f, -4.0f, TutorialUiLayer::fieldBottomGlow});
    
    parent.AddChild(*mGlowSceneObject);
}

void TutorialFieldBottomGlowAnimation::Update(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > pulseDuration) {
        mElapsedTime = 0.0f;
    }

    auto sineOfT = std::sin(mElapsedTime * 2.0f * 3.1415f / pulseDuration);
    auto opactiy = glowOpacity + pulseAmplitude * sineOfT;
    if (opactiy < 0.0f) {
        opactiy = 0.0f;
    }

    mGlowRenderable->GetMaterial().SetOpacity(opactiy);
}
