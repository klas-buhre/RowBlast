#include "MenuButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"
#include "SphereMesh.hpp"
#include "GuiView.hpp"
#include "Font.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace BlocksGame;

MenuButton::MenuButton(Pht::IEngine& engine,
                       Pht::GuiView& view,
                       const Pht::Vec3& position,
                       const Pht::Vec2& size,
                       const Pht::Vec2& inputSize,
                       const Style& style) :
    mView {view},
    mAudio {engine.GetAudio()},
    mPosition {position} {
    
    Pht::Material material {style.mColor};
    material.SetOpacity(style.mOpacity);
    
    auto& sceneManager {engine.GetSceneManager()};
    
    auto sceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                       material,
                                       view.GetSceneResources())
    };
    sceneObject->SetPosition(position);
    
    mButton = std::make_unique<Pht::Button>(*sceneObject, inputSize, engine);
    
    AddSceneObject(std::move(sceneObject));
    
    auto onDownFunction {[this, style] () {
        for (auto* sceneObject: mSceneObjects) {
            sceneObject->SetScale(style.mPressedScale);
            
            if (auto* renderable {sceneObject->GetRenderable()}) {
                renderable->GetMaterial().SetAmbient(style.mSelectedColor);
            }
            
            if (mText) {
                mText->mProperties.mScale = style.mPressedScale;
                Pht::Vec2 textLocalPosition {mTextLocalPosition * style.mPressedScale};
                mText->mPosition = textLocalPosition + Pht::Vec2 {mPosition.x, mPosition.y};
            }
        }
    }};
    
    mButton->SetOnDown(onDownFunction);
    
    auto onUpFunction {[this, style] () {
        for (auto sceneObject: mSceneObjects) {
            sceneObject->SetScale(1.0f);
            
            if (auto* renderable {sceneObject->GetRenderable()}) {
                renderable->GetMaterial().SetAmbient(style.mColor);
            }
            
            if (mText) {
                mText->mProperties.mScale = 1.0f;
                mText->mPosition = mTextLocalPosition + Pht::Vec2 {mPosition.x, mPosition.y};
            }
        }
    }};
    
    mButton->SetOnUpInside(onUpFunction);
    mButton->SetOnUpOutside(onUpFunction);
    mButton->SetOnMoveOutside(onUpFunction);
    
    if (style.mIsRounded) {
        auto leftSceneObject {
            sceneManager.CreateSceneObject(Pht::SphereMesh {size.y / 2.0f},
                                           material,
                                           view.GetSceneResources())
        };
        leftSceneObject->SetPosition(position + Pht::Vec3 {-size.x / 2.0f, 0.0f, 0.0f});
        AddSceneObject(std::move(leftSceneObject));
        
        auto rightSceneObject {
            sceneManager.CreateSceneObject(Pht::SphereMesh {size.y / 2.0f},
                                           material,
                                           view.GetSceneResources())
        };
        rightSceneObject->SetPosition(position + Pht::Vec3 {size.x / 2.0f, 0.0f, 0.0f});
        AddSceneObject(std::move(rightSceneObject));
    }
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

void MenuButton::SetText(std::unique_ptr<Pht::Text> text) {
    mText = text.get();
    mTextLocalPosition = text->mPosition;
    text->mPosition += Pht::Vec2 {mPosition.x, mPosition.y};
    mView.AddText(std::move(text));
}

bool MenuButton::IsClicked(const Pht::TouchEvent& event) const {
    auto isClicked {mButton->IsClicked(event, mView.GetMatrix())};
    
    if (isClicked) {
        mAudio.PlaySound(CommonResources::mBlipSound);
    }
    
    return isClicked;
}

void MenuButton::AddSceneObject(std::unique_ptr<Pht::SceneObject> sceneObject) {
    mSceneObjects.push_back(sceneObject.get());
    mView.AddSceneObject(std::move(sceneObject));
}
