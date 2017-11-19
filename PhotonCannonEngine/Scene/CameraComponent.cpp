#include "CameraComponent.hpp"

#include "Fnv1Hash.hpp"

using namespace Pht;

const ComponentId CameraComponent::id {Hash::Fnv1a("CameraComponent")};

CameraComponent::CameraComponent(SceneObject& sceneObject) :
    mSceneObject {sceneObject} {}

SceneObject& CameraComponent::GetSceneObject() {
    return mSceneObject;
}

void CameraComponent::SetTarget(const Vec3& target, const Vec3& up) {
    mTarget = target;
    mUp = up;
}
