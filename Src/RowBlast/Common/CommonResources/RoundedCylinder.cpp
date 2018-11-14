#include "RoundedCylinder.hpp"

// Engine includes.
#include "Scene.hpp"
#include "Material.hpp"
#include "SceneObject.hpp"
#include "CylinderMesh.hpp"
#include "SphereMesh.hpp"

using namespace RowBlast;

namespace {
    void CreateSphere(Pht::Scene& scene,
                      Pht::SceneObject& parentObject,
                      const Pht::Vec3& position,
                      float radius,
                      float angle,
                      const Pht::Material& material) {
        Pht::SphereMesh sphereMesh {radius};
        sphereMesh.SetUpperBound({Pht::Pi / 2.0f, Pht::TwoPi});
        
        auto& sphere {scene.CreateSceneObject(sphereMesh, material)};
        auto& transform {sphere.GetTransform()};
        transform.SetPosition(position);
        transform.SetRotation({0.0f, 0.0f, angle});
        parentObject.AddChild(sphere);
    }
}

Pht::SceneObject& RowBlast::CreateRoundedCylinder(Pht::Scene& scene,
                                                  Pht::SceneObject& parentObject,
                                                  const Pht::Vec3& position,
                                                  const Pht::Vec2& size,
                                                  float opacity,
                                                  const Pht::Color& ambient,
                                                  const Pht::Color& diffuse) {
    auto& sceneObject {scene.CreateSceneObject()};
    sceneObject.GetTransform().SetPosition(position);
    parentObject.AddChild(sceneObject);
    
    Pht::EnvMapTextureFilenames envMapTextures {
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg"
    };

    Pht::Color specular {1.0f, 1.0f, 1.0f};
    const auto shininess {30.0f};
    const auto reflectivity {0.84f};
    Pht::Material material {
        envMapTextures,
        ambient,
        diffuse,
        specular,
        shininess,
        reflectivity
    };
    material.SetOpacity(opacity);
    
    auto& cylinder {
        scene.CreateSceneObject(Pht::CylinderMesh {size.y / 2.0f, size.x, false}, material)
    };
    auto& cylinderTransform {cylinder.GetTransform()};
    cylinderTransform.SetRotation({0.0f, 0.0f, 90.0f});
    sceneObject.AddChild(cylinder);
    
    CreateSphere(scene, sceneObject, {size.x / 2.0f, 0.0f, 0.0f}, size.y / 2.0f, -90.0f, material);
    CreateSphere(scene, sceneObject, {-size.x / 2.0f, 0.0f, 0.0f}, size.y / 2.0f, 90.0f, material);
    
    return sceneObject;
}
