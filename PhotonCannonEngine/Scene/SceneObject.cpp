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
    // mTransform *= Mat4::Translate(translation.x, translation.y, translation.z);
    mMatrix.w.x += translation.x;
    mMatrix.w.y += translation.y;
    mMatrix.w.z += translation.z;
    mPosition += translation;
}

void SceneObject::RotateX(float degrees) {
    mMatrix *= Mat4::RotateX(degrees);
}

void SceneObject::RotateY(float degrees) {
    mMatrix *= Mat4::RotateY(degrees);
}

void SceneObject::RotateZ(float degrees) {
    mMatrix *= Mat4::RotateZ(degrees);
}

void SceneObject::Scale(float scale) {
    mMatrix *= Mat4::Scale(scale);
}

void SceneObject::ResetTransform() {
    mMatrix = Mat4 {};
    mPosition = {0.0f, 0.0f, 0.0f};
}

void SceneObject::AddChild(SceneObject& child) {
    mChildren.push_back(&child);
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
