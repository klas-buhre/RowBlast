#include "SettingsButtonView.hpp"

// Engine includes.
#include "SphereMesh.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Color circleColor {0.0f, 0.5f, 1.0f};
    const Pht::Color circleClickedColor {0.25f, 0.75f, 1.0f};
    const Pht::Vec3 circlePosition {0.0f, 0.0f, -2.0f};
}

SettingsButtonView::SettingsButtonView(Pht::IEngine& engine) {
    auto hudFrustumSize {engine.GetRenderer().GetHudFrustumSize()};
    SetPosition({-hudFrustumSize.x / 2.0f + 1.5f, -hudFrustumSize.y / 2.0f + 1.5f});
    
    Pht::Material circleMaterial {circleColor};
    circleMaterial.SetOpacity(0.8f);
    auto circle {engine.CreateRenderableObject(Pht::SphereMesh {1.0f}, circleMaterial)};
    auto circleSceneObject {std::make_unique<Pht::SceneObject>(std::move(circle))};
    circleSceneObject->Translate(circlePosition);
    auto& circleMaterialCapture {circleSceneObject->GetRenderable().GetMaterial()};
    auto& circleSceneObjectCapture {*circleSceneObject};
    
    Pht::Vec2 buttonSize {60.0f, 60.0f};
    mButton = std::make_unique<Pht::Button>(*circleSceneObject, buttonSize, engine);
    
    Pht::Material gearIconMaterial {"settings.png"};
    gearIconMaterial.SetBlend(Pht::Blend::Yes);
    auto gearIcon {std::make_unique<Pht::SceneObject>(
        engine.CreateRenderableObject(Pht::QuadMesh {1.4f, 1.4f}, gearIconMaterial))};
    auto& gearIconCapture {*gearIcon};
    
    auto selectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleClickedColor);
        circleSceneObjectCapture.ResetMatrix();
        circleSceneObjectCapture.Scale(1.35f);
        circleSceneObjectCapture.Translate(circlePosition);
        gearIconCapture.ResetMatrix();
        gearIconCapture.Scale(1.35f);
    }};
    
    mButton->SetOnDown(selectFunction);
    
    auto deselectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleColor);
        circleSceneObjectCapture.ResetMatrix();
        circleSceneObjectCapture.Translate(circlePosition);
        gearIconCapture.ResetMatrix();
    }};
    
    mButton->SetOnUpInside(deselectFunction);
    mButton->SetOnUpOutside(deselectFunction);
    mButton->SetOnMoveOutside(deselectFunction);
    
    AddSceneObject(std::move(circleSceneObject));
    AddSceneObject(std::move(gearIcon));
}
