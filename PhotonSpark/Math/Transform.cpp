#include "Transform.hpp"

using namespace Pht;

void Transform::Reset() {
    mPosition = {0.0f, 0.0f, 0.0f};
    mScale = {1.0f, 1.0f, 1.0f};
    mRotation = {0.0f, 0.0f, 0.0f};
    mHasChanged = true;
}

void Transform::SetPosition(const Vec3& position) {
    mPosition = position;
    mHasChanged = true;
}

void Transform::SetScale(const Vec3& scale) {
    mScale = scale;
    mHasChanged = true;
}

void Transform::SetScale(float scale) {
    mScale = Vec3 {scale, scale, scale};
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
    result.x.y *= mScale.x;
    result.x.z *= mScale.x;

    result.y.x *= mScale.y;
    result.y.y *= mScale.y;
    result.y.z *= mScale.y;
    
    result.z.x *= mScale.z;
    result.z.y *= mScale.z;
    result.z.z *= mScale.z;

    result.w.x = mPosition.x;
    result.w.y = mPosition.y;
    result.w.z = mPosition.z;
    
    return result;
}
