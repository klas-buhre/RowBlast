#include "SceneObject.hpp"

#include <assert.h>

using namespace Pht;

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
    mMatrix *= Mat4::Translate(translation.x, translation.y, translation.z);
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

void SceneObject::ResetMatrix() {
    mMatrix = Mat4 {};
    mPosition = {0.0f, 0.0f, 0.0f};
}

const RenderableObject& SceneObject::GetRenderable() const {
    assert(mRenderable);
    return *mRenderable;
}

RenderableObject& SceneObject::GetRenderable() {
    assert(mRenderable);
    return *mRenderable;
}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child) {
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
