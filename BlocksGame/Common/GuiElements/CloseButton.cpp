#include "CloseButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "SphereMesh.hpp"
#include "GuiView.hpp"
#include "Font.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Color color {1.0f, 0.3f, 0.3};
    const Pht::Color& selectedColor {1.0f, 0.6f, 0.6f};
    const float size {1.5f};
    const Pht::Vec2 inputSize {50.0f, 50.0f};
}

CloseButton::CloseButton(Pht::IEngine& engine,
                         Pht::GuiView& view,
                         const Pht::Vec3& position,
                         const Pht::TextProperties& textProperties) :
    mPosition {position},
    mAudio {engine.GetAudio()} {

    Pht::Material material {color};
    material.SetOpacity(0.8f);
    
    auto& sceneManager {engine.GetSceneManager()};
    auto sceneObject {
        sceneManager.CreateSceneObject(Pht::SphereMesh {size / 2.0f},
                                       material,
                                       view.GetSceneResources())
    };
    sceneObject->GetTransform().SetPosition(position);
    
    mButton = std::make_unique<Pht::Button>(*sceneObject, inputSize, engine);
    
    mSceneObject = sceneObject.get();
    view.AddSceneObject(std::move(sceneObject));
    
    auto selectFunction {[this] () {
        mSceneObject->GetRenderable()->GetMaterial().SetAmbient(selectedColor);
    }};
    
    mButton->SetOnDown(selectFunction);
    
    auto deselectFunction {[this] () {
        mSceneObject->GetRenderable()->GetMaterial().SetAmbient(color);
    }};
    
    mButton->SetOnUpInside(deselectFunction);
    mButton->SetOnUpOutside(deselectFunction);
    mButton->SetOnMoveOutside(deselectFunction);
    
    view.CreateText(position + Pht::Vec3 {-0.23f, -0.23f, UiLayer::text},
                    "X",
                    textProperties);
}

bool CloseButton::IsClicked(const Pht::TouchEvent& event) const {
    auto isClicked {mButton->IsClicked(event)};
    
    if (isClicked) {
        mAudio.PlaySound(CommonResources::mBlipSound);
    }
    
    return isClicked;
}
