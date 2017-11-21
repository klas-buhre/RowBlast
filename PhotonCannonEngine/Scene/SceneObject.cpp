#include "SceneObject.hpp"

#include <assert.h>

using namespace Pht;

SceneObject::SceneObject() {}

SceneObject::SceneObject(Name name) :
    mName {name} {}

SceneObject::SceneObject(std::shared_ptr<RenderableObject> renderable) :
    mRenderable {renderable} {
}

void SceneObject::SetRenderable(std::shared_ptr<RenderableObject> renderable) {
    assert(renderable);
    mRenderable = renderable;
}

void SceneObject::Translate(const Vec3& translation) {
    mTransform *= Mat4::Translate(translation.x, translation.y, translation.z);
    mPosition += translation;
}

void SceneObject::RotateX(float degrees) {
    mTransform *= Mat4::RotateX(degrees);
}

void SceneObject::RotateY(float degrees) {
    mTransform *= Mat4::RotateY(degrees);
}

void SceneObject::RotateZ(float degrees) {
    mTransform *= Mat4::RotateZ(degrees);
}

void SceneObject::Scale(float scale) {
    mTransform *= Mat4::Scale(scale);
}

void SceneObject::ResetTransform() {
    mTransform = Mat4 {};
    mPosition = {0.0f, 0.0f, 0.0f};
}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child) {
    child->SetParent(this);
    mChildren.push_back(std::move(child));
}

SceneObject* SceneObject::Find(Name name) {
    if (name == mName) {
        return this;
    }
    
    for (auto& child: mChildren) {
        auto* foundChild {child->Find(name)};
        
        if (foundChild) {
            return foundChild;
        }
    }
    
    return nullptr;
}
