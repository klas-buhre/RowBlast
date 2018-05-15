#include "MenuButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"
#include "SphereMesh.hpp"
#include "GuiView.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    std::unique_ptr<Pht::SceneObject> CreateMainSceneObject(Pht::ISceneManager& sceneManager,
                                                            Pht::GuiView& view,
                                                            const MenuButton::Style& style) {
        if (style.mRenderableObject) {
            return std::make_unique<Pht::SceneObject>(style.mRenderableObject);
        }

        Pht::Material material {style.mColor, style.mColor, {1.0f, 1.0f, 1.0f}, 20.0f};        
        return sceneManager.CreateSceneObject(Pht::ObjMesh {style.mMeshFilename},
                                              material,
                                              view.GetSceneResources());
    }
}

MenuButton::MenuButton(Pht::IEngine& engine,
                       Pht::GuiView& view,
                       const Pht::Vec3& position,
                       const Pht::Vec2& inputSize,
                       const Style& style) :
    mView {view},
    mAudio {engine.GetAudio()} {
    
    auto& sceneManager {engine.GetSceneManager()};
    auto sceneObject {CreateMainSceneObject(sceneManager, view, style)};
    
    sceneObject->GetTransform().SetPosition(position);
    
    mButton = std::make_unique<Pht::Button>(*sceneObject, inputSize, engine);
    
    AddSceneObject(std::move(sceneObject));
    
    if (style.mHasShadow) {
        Pht::Material shaddowMaterial {Pht::Color{0.4f, 0.4f, 0.4f}};
        shaddowMaterial.SetOpacity(0.16f);
        auto shadowSceneObject {
            sceneManager.CreateSceneObject(Pht::ObjMesh {style.mMeshFilename},
                                           shaddowMaterial,
                                           view.GetSceneResources())
        };
        shadowSceneObject->GetTransform().SetPosition(position - Pht::Vec3{0.15f, 0.15f, 0.1f});
        AddSceneObject(std::move(shadowSceneObject));
    }
    
    auto onDownFunction {[this, style] () {
        for (auto* sceneObject: mSceneObjects) {
            sceneObject->GetTransform().SetScale(style.mPressedScale);
            
            if (auto* renderable {sceneObject->GetRenderable()}) {
                if (renderable->GetMaterial().GetBlend() == Pht::Blend::No) {
                    renderable->GetMaterial().SetAmbient(style.mSelectedColor);
                }
            }
            
            if (auto* textComponent {sceneObject->GetComponent<Pht::TextComponent>()}) {
                textComponent->GetProperties().mScale = style.mPressedScale;
            }
        }
    }};
    
    mButton->SetOnDown(onDownFunction);
    
    auto onUpFunction {[this, style] () {
        for (auto sceneObject: mSceneObjects) {
            sceneObject->GetTransform().SetScale(1.0f);
            
            if (auto* renderable {sceneObject->GetRenderable()}) {
                if (renderable->GetMaterial().GetBlend() == Pht::Blend::No) {
                    renderable->GetMaterial().SetAmbient(style.mColor);
                }
            }

            if (auto* textComponent {sceneObject->GetComponent<Pht::TextComponent>()}) {
                textComponent->GetProperties().mScale = 1.0f;
            }
        }
    }};
    
    mButton->SetOnUpInside(onUpFunction);
    mButton->SetOnUpOutside(onUpFunction);
    mButton->SetOnMoveOutside(onUpFunction);
}

Pht::TextComponent& MenuButton::CreateText(const Pht::Vec3& position,
                                           const std::string& text,
                                           const Pht::TextProperties& properties) {
    auto sceneObject {std::make_unique<Pht::SceneObject>()};
    auto textComponent {std::make_unique<Pht::TextComponent>(*sceneObject, text, properties)};
    
    auto& retVal {*textComponent};
    sceneObject->SetComponent<Pht::TextComponent>(std::move(textComponent));
    sceneObject->GetTransform().SetPosition(position);
    mSceneObjects.front()->AddChild(*sceneObject);
    
    mSceneObjects.push_back(sceneObject.get());
    mView.GetSceneResources().AddSceneObject(std::move(sceneObject));
    return retVal;
}

bool MenuButton::IsClicked(const Pht::TouchEvent& event) const {
    auto isClicked {mButton->IsClicked(event)};
    
    if (isClicked) {
        mAudio.PlaySound(CommonResources::mBlipSound);
    }
    
    return isClicked;
}

void MenuButton::AddSceneObject(std::unique_ptr<Pht::SceneObject> sceneObject) {
    mSceneObjects.push_back(sceneObject.get());
    mView.AddSceneObject(std::move(sceneObject));
}
