#include "Keyframe.hpp"

using namespace Pht;

Keyframe::Keyframe(float time) :
    mTime {time} {}

void Keyframe::SetPosition(const Vec3& position) {
    mPosition = position;
}

void Keyframe::SetScale(const Vec3& scale) {
    mScale = scale;
}

void Keyframe::SetRotation(const Vec3& rotation) {
    mRotation = rotation;
}

void Keyframe::SetIsVisible(bool isVisible) {
    mIsVisible = isVisible;
}
