#include "GuiView.hpp"

#include "TextComponent.hpp"

using namespace Pht;

GuiView::GuiView() :
    mRoot {std::make_unique<Pht::SceneObject>()} {}

void GuiView::SetPosition(const Vec2& position) {
    mRoot->GetTransform().SetPosition({position.x, position.y, 0.0f});
}

void GuiView::SetIsActive(bool isActive) {
    if (isActive) {
        mRoot->SetIsVisible(true);
        mRoot->SetIsStatic(false);
    } else {
        mRoot->SetIsVisible(false);
        mRoot->SetIsStatic(true);
    }
}

void GuiView::AddSceneObject(std::unique_ptr<SceneObject> sceneObject) {
    mRoot->AddChild(*sceneObject);
    mSceneResources.AddSceneObject(std::move(sceneObject));
}

TextComponent& GuiView::CreateText(const Vec3& position,
                                   const std::string& text,
                                   const TextProperties& properties) {
    auto sceneObject {std::make_unique<SceneObject>()};
    auto textComponent {std::make_unique<TextComponent>(*sceneObject, text, properties)};
    
    auto& retVal {*textComponent};
    sceneObject->SetComponent<TextComponent>(std::move(textComponent));
    sceneObject->GetTransform().SetPosition(position);
    AddSceneObject(std::move(sceneObject));
    return retVal;
}

Vec2 GuiView::GetPosition() const {
    auto& position {mRoot->GetTransform().GetPosition()};
    return {position.x, position.y};
}

const Mat4& GuiView::GetMatrix() const {
    return mRoot->GetMatrix();
}

void GuiView::AddText(std::unique_ptr<Text> text) {
    mTexts.push_back(std::move(text));
}
