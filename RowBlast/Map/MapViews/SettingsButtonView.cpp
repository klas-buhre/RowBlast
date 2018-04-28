#include "SettingsButtonView.hpp"

// Engine includes.
#include "SphereMesh.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "ISceneManager.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const Pht::Color circleColor {0.0f, 0.5f, 1.0f};
    const Pht::Color circleClickedColor {0.25f, 0.75f, 1.0f};
}

SettingsButtonView::SettingsButtonView(Pht::IEngine& engine) {
    auto& renderer {engine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};
    
    Pht::Vec2 position {
        -hudFrustumSize.x / 2.0f + 1.5f,
        -hudFrustumSize.y / 2.0f + 1.5f + renderer.GetBottomPaddingHeight()
    };
    
    SetPosition(position);
    
    Pht::Material circleMaterial {circleColor};
    circleMaterial.SetOpacity(0.8f);
    
    auto& sceneManger {engine.GetSceneManager()};
    
    auto circleSceneObject {
        sceneManger.CreateSceneObject(Pht::SphereMesh {1.0f},
                                      circleMaterial,
                                      GetSceneResources())
    };
    circleSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    auto& circleMaterialCapture {circleSceneObject->GetRenderable()->GetMaterial()};
    auto& circleSceneObjectCapture {*circleSceneObject};
    
    Pht::Vec2 buttonSize {60.0f, 60.0f};
    mButton = std::make_unique<Pht::Button>(*circleSceneObject, buttonSize, engine);
    
    Pht::Material gearIconMaterial {"settings.png"};
    gearIconMaterial.SetBlend(Pht::Blend::Yes);
    auto gearIcon {
        sceneManger.CreateSceneObject(Pht::QuadMesh {1.4f, 1.4f},
                                      gearIconMaterial,
                                      GetSceneResources())
    };
    auto& gearIconCapture {*gearIcon};
    
    auto selectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleClickedColor);
        circleSceneObjectCapture.GetTransform().SetScale(1.35f);
        gearIconCapture.GetTransform().SetScale(1.35f);
    }};
    
    mButton->SetOnDown(selectFunction);
    
    auto deselectFunction {[&] () {
        circleMaterialCapture.SetAmbient(circleColor);
        circleSceneObjectCapture.GetTransform().SetScale(1.0f);
        gearIconCapture.GetTransform().SetScale(1.0f);
    }};
    
    mButton->SetOnUpInside(deselectFunction);
    mButton->SetOnUpOutside(deselectFunction);
    mButton->SetOnMoveOutside(deselectFunction);
    
    AddSceneObject(std::move(circleSceneObject));
    AddSceneObject(std::move(gearIcon));
}
