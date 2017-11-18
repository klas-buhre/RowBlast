#include "CameraComponent.hpp"

#include "Fnv1Hash.hpp"

using namespace Pht;

const ComponentId CameraComponent::id {Hash::Fnv1a("CameraComponent")};

void CameraComponent::SetTarget(const Vec3& target, const Vec3& up) {
    mTarget = target;
    mUp = up;
}
