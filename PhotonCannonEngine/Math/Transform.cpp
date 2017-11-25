#include "Transform.hpp"

using namespace Pht;

void Transform::SetPosition(const Vec3& position) {
    mPosition = position;
    mHasChanged = true;
}

void Transform::SetScale(const Vec3& scale) {
    mScale = scale;
    mHasChanged = true;
}

void Transform::SetRotation(const Vec3& rotation) {
    mRotation = rotation;
    mHasChanged = true;
}

void Transform::Translate(const Vec3& translation) {
    mPosition += translation;
    mHasChanged = true;
}

void Transform::Scale(const Vec3& scale) {
    mScale *= scale;
    mHasChanged = true;
}

void Transform::Rotate(const Vec3& rotation) {
    mRotation += rotation;
    mHasChanged = true;
}

Mat4 Transform::ToMatrix() {
    
}
