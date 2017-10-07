#include "GameHudView.hpp"

// Engine includes.
#include "SphereMesh.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Color circleColor {0.0f, 0.5f, 1.0f};
    const Pht::Color circleClickedColor {0.25f, 0.75f, 1.0f};
    const Pht::Vec3 circlePosition {0.0f, 0.0f, -2.0f};
}

GameHudView::GameHudView(Pht::IEngine& engine) {
    SetPosition({-5.2f, -11.85f});
    
    Pht::Material circleMaterial {circleColor};
    circleMaterial.SetOpacity(0.8f);
    
    auto circle {engine.CreateRenderableObject(Pht::SphereMesh {0.8f}, circleMaterial)};
    auto circleSceneObject {std::make_unique<Pht::SceneObject>(std::move(circle))};
    circleSceneObject->Translate(circlePosition);
    auto& circleMaterialCapture {circleSceneObject->GetRenderable().GetMaterial()};
    auto& circleSceneObjectCapture {*circleSceneObject};
    
    Pht::Vec2 buttonSize {55.0f, 55.0f};
    mPauseButton = std::make_unique<Pht::Button>(*circleSceneObject, buttonSize, engine);
    
    auto pauseSelectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleClickedColor);
        circleSceneObjectCapture.ResetMatrix();
        circleSceneObjectCapture.Scale(1.35f);
        circleSceneObjectCapture.Translate(circlePosition);
    }};
    
    mPauseButton->SetOnDown(pauseSelectFunction);
    
    auto pauseDeselectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleColor);
        circleSceneObjectCapture.ResetMatrix();
        circleSceneObjectCapture.Translate(circlePosition);
    }};
    
    mPauseButton->SetOnUpInside(pauseDeselectFunction);
    mPauseButton->SetOnUpOutside(pauseDeselectFunction);
    mPauseButton->SetOnMoveOutside(pauseDeselectFunction);
    
    AddSceneObject(std::move(circleSceneObject));
    
    Pht::Material barMaterial {Pht::Color{1.0f, 1.0f, 1.0f}};
    
    std::shared_ptr<Pht::RenderableObject> bar {
        engine.CreateRenderableObject(Pht::QuadMesh {0.16f, 0.65f}, barMaterial)
    };
    
    auto leftBarSceneObject {std::make_unique<Pht::SceneObject>(bar)};
    leftBarSceneObject->Translate({-0.16f, 0.0f, 0.0f});
    AddSceneObject(std::move(leftBarSceneObject));
    
    auto rightBarSceneObject {std::make_unique<Pht::SceneObject>(bar)};
    rightBarSceneObject->Translate({0.16f, 0.0f, 0.0f});
    AddSceneObject(std::move(rightBarSceneObject));
    
    auto switchButtonSceneObject {std::make_unique<Pht::SceneObject>(nullptr)};
    switchButtonSceneObject->SetIsVisible(false);
    switchButtonSceneObject->Translate({8.65f, 0.0f, 0.0f});
    
    Pht::Vec2 switchButtonSize {110.0f, 60.0f};
    mSwitchButton = std::make_unique<Pht::Button>(*switchButtonSceneObject, switchButtonSize, engine);
    
    AddSceneObject(std::move(switchButtonSceneObject));
}
