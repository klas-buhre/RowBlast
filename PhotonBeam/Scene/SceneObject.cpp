#include "SceneObject.hpp"

#include <assert.h>

using namespace Pht;

SceneObject::SceneObject() {}

SceneObject::SceneObject(Name name) :
    mName {name} {}

SceneObject::SceneObject(RenderableObject* renderable) :
    mRenderable {renderable} {
}

Vec3 SceneObject::GetWorldSpacePosition() const {
    auto& w {mMatrix.w};
    return {w.x, w.y, w.z};
}

void SceneObject::Update(bool parentMatrixChanged) {
    auto matrixWasChanged {false};
    
    if (mTransform.HasChanged() || parentMatrixChanged) {
        mMatrix = mTransform.ToMatrix();
        mTransform.SetHasChanged(false);
        matrixWasChanged = true;
        
        if (mParent) {
            mMatrix *= mParent->GetMatrix();
        }
    }
    
    for (auto* child: mChildren) {
        if (!child->mIsStatic) {
            child->Update(matrixWasChanged);
        }
    }
}

void SceneObject::AddChild(SceneObject& child) {
    child.mParent = this;
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

void SceneObject::SetLayer(int layerIndex) {
    mLayerMask = (1 << layerIndex);
}
