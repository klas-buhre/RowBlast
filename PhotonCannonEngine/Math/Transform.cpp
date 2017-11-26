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
    Mat4 result;
    
    if (mRotation.x != 0.0f) {
        result *= Mat4::RotateX(mRotation.x);
    }

    if (mRotation.y != 0.0f) {
        result *= Mat4::RotateY(mRotation.y);
    }

    if (mRotation.z != 0.0f) {
        result *= Mat4::RotateZ(mRotation.z);
    }

    result.x.x *= mScale.x;
    result.y.y *= mScale.y;
    result.z.z *= mScale.z;
    
    result.w.x = mPosition.x;
    result.w.y = mPosition.y;
    result.w.z = mPosition.z;
    
    return result;
}
