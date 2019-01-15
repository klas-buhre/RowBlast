#include "ScrollPanel.hpp"

using namespace Pht;

ScrollPanel::ScrollPanel() :
    mRoot {std::make_unique<Pht::SceneObject>()},
    mPanel {std::make_unique<Pht::SceneObject>()} {
    
    mRoot->AddChild(*mPanel);
}

void ScrollPanel::SetPosition(const Vec3& position) {
    mRoot->GetTransform().SetPosition(position);
}

void ScrollPanel::AddSceneObject(SceneObject& sceneObject) {
    mPanel->AddChild(sceneObject);
}
