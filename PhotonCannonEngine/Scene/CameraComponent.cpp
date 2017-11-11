#include "CameraComponent.hpp"

using namespace Pht;

void CameraComponent::SetTarget(const Vec3& target, const Vec3& up) {
    mTarget = target;
    mUp = up;
}
