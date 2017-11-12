#include "MenuButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"
#include "SphereMesh.hpp"
#include "GuiView.hpp"
#include "Font.hpp"

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
    
    auto centerQuad {engine.CreateRenderableObject(Pht::QuadMesh {size.x, size.y}, material)};
    auto sceneObject {std::make_unique<Pht::SceneObject>(std::move(centerQuad))};
    sceneObject->Translate(position);
    sceneObject->SetIsInFront(true);
    
    mButton = std::make_unique<Pht::Button>(*sceneObject, inputSize, engine);
    
    AddSceneObject(std::move(sceneObject));
    
    auto onDownFunction {[this, style] () {
        for (auto sceneObject: mSceneObjects) {
            auto position {sceneObject->GetPosition()};
            sceneObject->ResetTransform();
            sceneObject->Scale(style.mPressedScale);
            sceneObject->Translate(position);
            sceneObject->GetRenderable()->GetMaterial().SetAmbient(style.mSelectedColor);
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
            auto position {sceneObject->GetPosition()};
            sceneObject->ResetTransform();
            sceneObject->Translate(position);
            sceneObject->GetRenderable()->GetMaterial().SetAmbient(style.mColor);
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
        auto leftCircle {engine.CreateRenderableObject(Pht::SphereMesh {size.y / 2.0f}, material)};
        auto leftSceneObject {std::make_unique<Pht::SceneObject>(std::move(leftCircle))};
        leftSceneObject->Translate(position + Pht::Vec3 {-size.x / 2.0f, 0.0f, 0.0f});
        AddSceneObject(std::move(leftSceneObject));
        
        auto rightCircle {engine.CreateRenderableObject(Pht::SphereMesh {size.y / 2.0f}, material)};
        auto rightSceneObject {std::make_unique<Pht::SceneObject>(std::move(rightCircle))};
        rightSceneObject->Translate(position + Pht::Vec3 {size.x / 2.0f, 0.0f, 0.0f});
        AddSceneObject(std::move(rightSceneObject));
    }
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
