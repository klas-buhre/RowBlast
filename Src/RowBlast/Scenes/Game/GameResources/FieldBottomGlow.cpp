#include "FieldBottomGlow.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto glowWidthInCells = 9.2f;
    constexpr auto glowHeightInCells = 1.0f;
    constexpr auto glowOpacity = 0.275f;
    constexpr auto pulseAmplitude = 0.435f;
    constexpr auto pulseDuration = 1.25f;
}

FieldBottomGlow::FieldBottomGlow(Pht::IEngine& engine,
                                 GameScene& scene,
                                 const ScrollController& scrollController) :
    mScene {scene},
    mScrollController {scrollController} {

    Pht::Material glowMaterial;
    glowMaterial.SetBlend(Pht::Blend::Additive);
    glowMaterial.SetDepthTest(false);
    glowMaterial.SetOpacity(glowOpacity);
    
    auto& sceneManager = engine.GetSceneManager();
    auto cellSize = scene.GetCellSize();
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
}

void FieldBottomGlow::Init() {
    mScene.GetFieldBlocksContainer().AddChild(*mGlowSceneObject);
    GoToInactiveState();
    
    auto cellSize = mScene.GetCellSize();
    
    Pht::Vec3 positionInField {
        mScene.GetFieldWidth() / 2.0f,
        glowHeightInCells * cellSize / 2.0f,
        mScene.GetFieldBottomGlowZ()
    };

    mGlowSceneObject->GetTransform().SetPosition(positionInField);
}

void FieldBottomGlow::Update(float dt) {
    switch (mState) {
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            UpdateInInactiveState();
            break;
    }
}

void FieldBottomGlow::UpdateInActiveState(float dt) {
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
    
    if (mScrollController.GetLowestVisibleRow() > glowHeightInCells) {
        GoToInactiveState();
    }
}

void FieldBottomGlow::UpdateInInactiveState() {
    if (mScrollController.GetLowestVisibleRow() <= glowHeightInCells) {
        GoToActiveState();
    }
}

void FieldBottomGlow::GoToActiveState() {
    mState = State::Active;
    mElapsedTime = 0.0f;
    mGlowSceneObject->SetIsVisible(true);
}

void FieldBottomGlow::GoToInactiveState() {
    mState = State::Inactive;
    mGlowSceneObject->SetIsVisible(false);
}
