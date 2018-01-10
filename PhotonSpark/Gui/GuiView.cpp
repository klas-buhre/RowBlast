#include "GuiView.hpp"

#include <algorithm>

#include "SceneObject.hpp"
#include "Button.hpp"
#include "Font.hpp"

using namespace Pht;

GuiView::~GuiView() {}

void GuiView::SetPosition(const Vec2& position) {
    mPosition = position;
    mMatrix = Mat4::Translate(position.x, position.y, 0.0f);
}

void GuiView::AddSceneObject(std::unique_ptr<SceneObject> sceneObject) {
    mSceneObjects.push_back(std::move(sceneObject));
    std::sort(std::begin(mSceneObjects),
              std::end(mSceneObjects),
              [] (auto& a, auto& b) {
                  if (a->IsInFront() || b->IsInFront()) {
                      if (a->IsInFront()) {
                          return false;
                      } else {
                          return true;
                      }
                  } else {
                      return a->GetTransform().GetPosition().z < b->GetTransform().GetPosition().z;
                  }
              }
    );
}

void GuiView::AddText(std::unique_ptr<Text> text) {
    mTexts.push_back(std::move(text));
}
